/*
 * render.c
 *
 *  Created on: 2018-6-29
 *      Author: Brant
 *
 * THis file is a port of FreeType project
 *
 */

#include <gdi/gui_interface.h>
#include <gdi/font/render.h>
#include <gdi/font/font_metrics.h>


#define FT_CURVE_TAG( flag )  ( flag & 3 )

#define FT_CURVE_TAG_ON            1
#define FT_CURVE_TAG_CONIC         0
#define FT_CURVE_TAG_CUBIC         2

#define ONE_PIXEL       (256)
#define PIXEL_MASK      ( -1L << 8 )
#define TRUNC( x )      ( (TCoord)( (x) >> 8 ) )
#define SUBPIXELS( x )  ( (TPos)(x) << 8 )
#define FLOOR( x )      ( (x) & -ONE_PIXEL )
#define CEILING( x )    ( ( (x) + ONE_PIXEL - 1 ) & -ONE_PIXEL )
#define ROUND( x )      ( ( (x) + ONE_PIXEL / 2 ) & -ONE_PIXEL )

#define UPSCALE( x )    ( (x) << 2 )
#define DOWNSCALE( x )  ( (x) >> 2 )


static PCell gray_find_cell(PWorker worker)
{
	PCell *pcell, cell;
	TPos x = worker->ex;

	if (x > worker->count_ex)
		x = worker->count_ex;

	pcell = &worker->ycells[worker->ey];
	for (;;)
	{
		cell = *pcell;
		if (cell == NULL || cell->x > x)
			break;

		if (cell->x == x)
			goto Exit;

		pcell = &cell->next;
	}

	if (worker->num_cells >= worker->max_cells)
	{
		return NULL;
//		longjmp(worker->jump_buffer, 1);
	}

	cell = worker->cells + worker->num_cells++;
	cell->x = x;
	cell->area = 0;
	cell->cover = 0;

	cell->next = *pcell;
	*pcell = cell;

Exit:
	return cell;
}

static int gray_record_cell(PWorker worker)
{
	PCell cell;

	if (!worker->invalid && (worker->area | worker->cover))
	{
		cell = gray_find_cell(worker);
		if (cell == NULL)
		{
			return ErrRaster_Memory_Overflow;
		}
		cell->area += worker->area;
		cell->cover += worker->cover;
	}

	return ErrRaster_None;
}

static int gray_set_cell(PWorker worker, TCoord ex, TCoord ey)
{
	int error = ErrRaster_None;

	ey -= worker->min_ey;

	if (ex > worker->max_ex)
		ex = worker->max_ex;

	ex -= worker->min_ex;
	if (ex < 0)
		ex = -1;

	/* are we moving to a different cell ? */
	if (ex != worker->ex || ey != worker->ey)
	{
		/* record the current one if it is valid */
		if (!worker->invalid)
		{
			error = gray_record_cell(worker);
			if (error) { return error;}
		}

		worker->area = 0;
		worker->cover = 0;
	}

	worker->ex = ex;
	worker->ey = ey;
	worker->invalid = ((unsigned)ey >= (unsigned)worker->count_ey || ex >= worker->count_ex);

	return error;
}

static int gray_start_cell(PWorker worker, TCoord ex, TCoord ey)
{
	if (ex > worker->max_ex)
		ex = (TCoord)(worker->max_ex);

	if (ex < worker->min_ex)
		ex = (TCoord)(worker->min_ex - 1);

	worker->area = 0;
	worker->cover = 0;
	worker->ex = ex - worker->min_ex;
	worker->ey = ey - worker->min_ey;
	worker->last_ey = SUBPIXELS( ey );
	worker->invalid = 0;

	return gray_set_cell(worker, ex, ey);
}


static int gray_move_to(const FT_Vector* to, PWorker worker)
{
	TPos x, y;
	int error = ErrRaster_None;

	/* record current cell, if any */
	error = gray_record_cell(worker);
	if (error) { return error;}

	/* start to a new position */
	x = UPSCALE(to->x);
	y = UPSCALE(to->y);

	error = gray_start_cell(worker, TRUNC(x), TRUNC(y));
	if (error) { return error;}

	worker->x = x;
	worker->y = y;

	return error;
}

static int gray_render_scanline(PWorker worker, TCoord ey, TPos x1, TCoord y1, TPos x2, TCoord y2)
{
	TCoord ex1, ex2, fx1, fx2, delta, mod, lift, rem;
	long p, first, dx;
	int incr;
	int error = ErrRaster_None;

	dx = x2 - x1;

	ex1 = TRUNC( x1);
	ex2 = TRUNC( x2);
	fx1 = (TCoord)(x1 - SUBPIXELS( ex1));
	fx2 = (TCoord)(x2 - SUBPIXELS( ex2));

	/* trivial case.  Happens often */
	if (y1 == y2)
	{
		return gray_set_cell(worker, ex2, ey);
	}

	/* everything is located in a single cell.  That is easy! */
	/*                                                        */
	if (ex1 == ex2)
	{
		delta = y2 - y1;
		worker->area += (TArea)((fx1 + fx2) * delta);
		worker->cover += delta;
		return ErrRaster_None;
	}

	/* ok, we'll have to render a run of adjacent cells on the same */
	/* scanline...                                                  */
	/*                                                              */
	p = (ONE_PIXEL - fx1) * (y2 - y1);
	first = ONE_PIXEL;
	incr = 1;

	if (dx < 0)
	{
		p = fx1 * (y2 - y1);
		first = 0;
		incr = -1;
		dx = -dx;
	}

	delta = (TCoord)(p / dx);
	mod = (TCoord)(p % dx);
	if (mod < 0)
	{
		delta--;
		mod += (TCoord)dx;
	}

	worker->area += (TArea)((fx1 + first) * delta);
	worker->cover += delta;

	ex1 += incr;
	error = gray_set_cell(worker, ex1, ey);
	if (error) { return error;}
	y1 += delta;

	if (ex1 != ex2)
	{
		p = ONE_PIXEL * (y2 - y1 + delta);
		lift = (TCoord)(p / dx);
		rem = (TCoord)(p % dx);
		if (rem < 0)
		{
			lift--;
			rem += (TCoord)dx;
		}

		mod -= (int)dx;

		while (ex1 != ex2)
		{
			delta = lift;
			mod += rem;
			if (mod >= 0)
			{
				mod -= (TCoord)dx;
				delta++;
			}

			worker->area += (TArea)(ONE_PIXEL * delta);
			worker->cover += delta;
			y1 += delta;
			ex1 += incr;
			error = gray_set_cell(worker, ex1, ey);
			if (error) { return error;}
		}
	}

	delta = y2 - y1;
	worker->area += (TArea)((fx2 + ONE_PIXEL - first) * delta);
	worker->cover += delta;

	return ErrRaster_None;
}


static int gray_render_line(PWorker worker, TPos to_x, TPos to_y)
{
	TCoord ey1, ey2, fy1, fy2, mod;
	TPos dx, dy, x, x2;
	long p, first;
	int delta, rem, lift, incr;
	int error = ErrRaster_None;

	ey1 = TRUNC( worker->last_ey );
	ey2 = TRUNC( to_y ); /* if (ey2 >= worker->max_ey) ey2 = worker->max_ey-1; */
	fy1 = (TCoord)(worker->y - worker->last_ey);
	fy2 = (TCoord)(to_y - SUBPIXELS( ey2 ));

	dx = to_x - worker->x;
	dy = to_y - worker->y;

	/* : we should do something about the trivial case where dx == 0, */
	/*      as it happens very often!                                 */

	/* perform vertical clipping */
	{
		TCoord min, max;

		min = ey1;
		max = ey2;
		if (ey1 > ey2)
		{
			min = ey2;
			max = ey1;
		}
		if (min >= worker->max_ey || max < worker->min_ey)
			goto End;
	}

	/* everything is on a single scanline */
	if (ey1 == ey2)
	{
		error = gray_render_scanline(worker, ey1, worker->x, fy1, to_x, fy2);
		if (error) { return error;}
		goto End;
	}

  /* vertical line - avoid calling gray_render_scanline */
	incr = 1;

	if (dx == 0)
	{
		TCoord ex = TRUNC( worker->x );
		TCoord two_fx = (TCoord)((worker->x - SUBPIXELS( ex )) << 1);
		TArea area;

		first = ONE_PIXEL;
		if (dy < 0)
		{
			first = 0;
			incr = -1;
		}

		delta = (int)(first - fy1);
		worker->area += (TArea)two_fx * delta;
		worker->cover += delta;
		ey1 += incr;

		error = gray_set_cell(worker, ex, ey1);
		if (error) { return error;}

		delta = (int)(first + first - ONE_PIXEL);
		area = (TArea)two_fx * delta;
		while (ey1 != ey2)
		{
			worker->area += area;
			worker->cover += delta;
			ey1 += incr;

			error = gray_set_cell(worker, ex, ey1);
			if (error) { return error;}
		}

		delta = (int)(fy2 - ONE_PIXEL + first);
		worker->area += (TArea)two_fx * delta;
		worker->cover += delta;

		goto End;
	}

	/* ok, we have to render several scanlines */
	p = (ONE_PIXEL - fy1) * dx;
	first = ONE_PIXEL;
	incr = 1;

	if (dy < 0)
	{
		p = fy1 * dx;
		first = 0;
		incr = -1;
		dy = -dy;
	}

	delta = (int)(p / dy);
	mod = (int)(p % dy);
	if (mod < 0)
	{
		delta--;
		mod += (TCoord)dy;
	}

	x = worker->x + delta;
	error = gray_render_scanline(worker, ey1, worker->x, fy1, x, (TCoord)first);
	if (error) { return error;}

	ey1 += incr;
	error = gray_set_cell(worker, TRUNC( x ), ey1);
	if (error) { return error;}

	if (ey1 != ey2)
	{
		p = ONE_PIXEL * dx;
		lift = (int)(p / dy);
		rem = (int)(p % dy);
		if (rem < 0)
		{
			lift--;
			rem += (int)dy;
		}
		mod -= (int)dy;

		while (ey1 != ey2)
		{
			delta = lift;
			mod += rem;
			if (mod >= 0)
			{
				mod -= (int)dy;
				delta++;
			}

			x2 = x + delta;
			error = gray_render_scanline(worker, ey1, x, (TCoord)(ONE_PIXEL - first), x2, (TCoord)first);
			if (error) { return error;}
			x = x2;

			ey1 += incr;
			error = gray_set_cell(worker, TRUNC( x ), ey1);
			if (error) { return error;}
		}
	}

	error = gray_render_scanline(worker, ey1, x, (TCoord)(ONE_PIXEL - first), to_x, fy2);
	if (error) { return error;}

End:
	worker->x = to_x;
	worker->y = to_y;
	worker->last_ey = SUBPIXELS( ey2 );

	return error;
}


static int gray_line_to(const FT_Vector* to, PWorker worker)
{
	return gray_render_line(worker, UPSCALE( to->x ), UPSCALE( to->y ));
}

static void gray_split_conic(FT_Vector* base)
{
	TPos a, b;

	base[4].x = base[2].x;
	b = base[1].x;
	a = base[3].x = (base[2].x + b) / 2;
	b = base[1].x = (base[0].x + b) / 2;
	base[2].x = (a + b) / 2;

	base[4].y = base[2].y;
	b = base[1].y;
	a = base[3].y = (base[2].y + b) / 2;
	b = base[1].y = (base[0].y + b) / 2;
	base[2].y = (a + b) / 2;
}

static int gray_render_conic(PWorker worker, const FT_Vector* control, const FT_Vector* to)
{
	TPos dx, dy;
	TPos min, max, y;
	int top, level;
	int* levels = NULL;
	FT_Vector* arc;
	int error = ErrRaster_None;

	arc = worker->bez_stack;
	arc[0].x = UPSCALE( to->x );
	arc[0].y = UPSCALE( to->y );
	arc[1].x = UPSCALE( control->x );
	arc[1].y = UPSCALE( control->y );
	arc[2].x = worker->x;
	arc[2].y = worker->y;
	top = 0;

	dx = FT_ABS(arc[2].x + arc[0].x - 2 * arc[1].x);
	dy = FT_ABS(arc[2].y + arc[0].y - 2 * arc[1].y);
	if (dx < dy)
		dx = dy;

	if (dx < ONE_PIXEL / 4)
		goto Draw;

  /* short-cut the arc that crosses the current band */
	min = max = arc[0].y;

	y = arc[1].y;
	if (y < min)
		min = y;
	if (y > max)
		max = y;

	y = arc[2].y;
	if (y < min)
		min = y;
	if (y > max)
		max = y;

	if (TRUNC( min ) >= worker->max_ey || TRUNC( max ) < worker->min_ey)
		goto Draw;

	level = 0;
	do
	{
		dx >>= 2;
		level++;
	} while (dx > ONE_PIXEL / 4);

	levels = worker->lev_stack;
	levels[0] = level;

	do
	{
		level = levels[top];
		if (level > 0)
		{
			gray_split_conic(arc);
			arc += 2;
			top++;
			levels[top] = levels[top - 1] = level - 1;
			continue;
		}

Draw:
		error = gray_render_line(worker, arc[0].x, arc[0].y);
		if (error) { return error;}
		top--;
		arc -= 2;

	} while (top >= 0);

  return error;
}

static int gray_conic_to(const FT_Vector* control, const FT_Vector* to, PWorker worker)
{
	return gray_render_conic(worker, control, to);
}

//#define SCALED( x )  ( ( (x) << shift ) - delta )
/* for this render, both shift and delta is 0, so refine it as below */
#define SCALED(x)		(x)
static int FT_Outline_Decompose(FT_Outline* outline, PWorker worker)
{
	FT_Vector v_last;
	FT_Vector v_control;
	FT_Vector v_start;

	FT_Vector* point;
	FT_Vector* limit;
	char* tags;

	int error;

	FT_Int n; /* index of contour in outline     */
	FT_UInt first; /* index of first point in contour */
	FT_Int tag; /* current point's state           */

	if (!outline || !worker)
		return ErrRaster_Invalid_Argument;

	first = 0;

	for (n = 0; n < outline->n_contours; n++)
	{
		FT_Int last; /* index of last point in contour */

		last = outline->contours[n];
		if (last < 0)
			return ErrRaster_Invalid_Outline;
		limit = outline->points + last;

		v_start = outline->points[first];
		v_last = outline->points[last];

		v_control = v_start;

		point = outline->points + first;
		tags = outline->tags + first;
		tag = FT_CURVE_TAG(tags[0]);

		/* A contour cannot start with a cubic control point! */
		if (tag == FT_CURVE_TAG_CUBIC)
			return ErrRaster_Invalid_Outline;

		/* check first point to determine origin */
		if (tag == FT_CURVE_TAG_CONIC)
		{
			/* first point is conic control.  Yes, this happens. */
			if (FT_CURVE_TAG(outline->tags[last]) == FT_CURVE_TAG_ON)
			{
				/* start at last point if it is on the curve */
				v_start = v_last;
				limit--;
			}
			else
			{
				/* if both first and last points are conic,         */
				/* start at their middle and record its position    */
				/* for closure                                      */
				v_start.x = (v_start.x + v_last.x) / 2;
				v_start.y = (v_start.y + v_last.y) / 2;

				v_last = v_start;
			}
			point--;
			tags--;
		}

		error = gray_move_to(&v_start, worker);
		if (error) {return error;}

		while (point < limit)
		{
			point++;
			tags++;

			tag = FT_CURVE_TAG(tags[0]);
			switch (tag)
			{
				case FT_CURVE_TAG_ON: /* emit a single line_to */
				{
					FT_Vector vec;

					vec.x = SCALED( point->x );
					vec.y = SCALED( point->y );

					error = gray_line_to(&vec, worker);
					if (error) {return error;}
					continue;
				}

				case FT_CURVE_TAG_CONIC: /* consume conic arcs */
					v_control.x = SCALED( point->x );
					v_control.y = SCALED( point->y );

Do_Conic:
					if (point < limit)
					{
						FT_Vector vec;
						FT_Vector v_middle;

						point++;
						tags++;
						tag = FT_CURVE_TAG(tags[0]);

						vec.x = SCALED( point->x );
						vec.y = SCALED( point->y );

						if (tag == FT_CURVE_TAG_ON)
						{
							error = gray_conic_to(&v_control, &vec, worker);
							if (error) {return error;}
							continue;
						}

						if (tag != FT_CURVE_TAG_CONIC)
							return ErrRaster_Invalid_Outline;

						v_middle.x = (v_control.x + vec.x) / 2;
						v_middle.y = (v_control.y + vec.y) / 2;

						error = gray_conic_to(&v_control, &v_middle, worker);
						if (error) {return error;}

						v_control = vec;
						goto Do_Conic;
					}

					error = gray_conic_to(&v_control, &v_start, worker);
					goto Close;

				default: /* FT_CURVE_TAG_CUBIC */
					goto Close;
			}
		}

		/* close the contour with a line segment */
		error = gray_line_to(&v_start, worker);

Close:
		if (error) {return error;}

		first = last + 1;
	}
	return ErrRaster_None;
}

static void gray_compute_cbox(PWorker worker)
{
	FT_Outline* outline = &worker->outline;
	FT_Vector* vec = outline->points;
	FT_Vector* limit = vec + outline->n_points;

	if (outline->n_points <= 0)
	{
		worker->min_ex = worker->max_ex = 0;
		worker->min_ey = worker->max_ey = 0;
		return;
	}

	worker->min_ex = worker->max_ex = vec->x;
	worker->min_ey = worker->max_ey = vec->y;

	vec++;

	for (; vec < limit; vec++)
	{
		TPos x = vec->x;
		TPos y = vec->y;

		if (x < worker->min_ex)
			worker->min_ex = x;
		if (x > worker->max_ex)
			worker->max_ex = x;
		if (y < worker->min_ey)
			worker->min_ey = y;
		if (y > worker->max_ey)
			worker->max_ey = y;
	}

	/* truncate the bounding box to integer pixels */worker->min_ex = worker->min_ex >> 6;
	worker->min_ey = worker->min_ey >> 6;
	worker->max_ex = (worker->max_ex + 63) >> 6;
	worker->max_ey = (worker->max_ey + 63) >> 6;
}

static void gray_hline(PWorker worker, TCoord x, TCoord y, TPos area, TCoord acount)
{
	FT_Span* span;
	int count;
	int coverage;

	/* compute the coverage line's coverage, depending on the    */
	/* outline fill rule                                         */
	/*                                                           */
	/* the coverage percentage is area/(PIXEL_BITS*PIXEL_BITS*2) */
	/*                                                           */
//  coverage = (int)( area >> ( 8 * 2 + 1 - 8 ) );
	coverage = (int)(area >> 9);
	/* use range 0..256 */
	if (coverage < 0)
		coverage = -coverage;

	/* normal non-zero winding rule */
	if (coverage >= 256)
		coverage = 255;

	y += (TCoord)worker->min_ey;
	x += (TCoord)worker->min_ex;

	/* FT_Span.x is a 16-bit short, so limit our coordinates appropriately */
	if (x >= 32767)
		x = 32767;

	/* FT_Span.y is an integer, so limit our coordinates appropriately */
	if (y >= 0x7fffffff)
		y = 0x7fffffff;

	if (coverage)
	{
		/* see whether we can add this span to the current list */
		count = worker->num_gray_spans;
		span = worker->gray_spans + count - 1;
		if (count > 0 && worker->span_y == y && (int)span->x + span->len == (int)x && span->coverage == coverage)
		{
			span->len = (unsigned short)(span->len + acount);
			return;
		}

		if (worker->span_y != y || count >= FT_MAX_GRAY_SPANS)
		{
			if (worker->render_span && count > 0)
				worker->render_span(worker->span_y, count, worker->gray_spans, worker->render_span_data);

			worker->num_gray_spans = 0;
			worker->span_y = (int)y;

			count = 0;
			span = worker->gray_spans;
		}
		else
			span++;

		/* add a gray span to the current list */
		span->x = (short)x;
		span->len = (unsigned short)acount;
		span->coverage = (unsigned char)coverage;

		worker->num_gray_spans++;
	}
}


static void gray_sweep(PWorker worker)
{
	int yindex;

	if (worker->num_cells == 0)
		return;

	worker->num_gray_spans = 0;

	for (yindex = 0; yindex < worker->ycount; yindex++)
	{
		PCell cell = worker->ycells[yindex];
		TCoord cover = 0;
		TCoord x = 0;

		for (; cell != NULL; cell = cell->next)
		{
			TPos area;

			if (cell->x > x && cover != 0)
				gray_hline(worker, x, yindex, cover * (ONE_PIXEL * 2), cell->x - x);

			cover += cell->cover;
			area = cover * (ONE_PIXEL * 2) - cell->area;

			if (area != 0 && cell->x >= 0)
				gray_hline(worker, cell->x, yindex, area, 1);

			x = cell->x + 1;
		}

		if (cover != 0)
			gray_hline(worker, x, yindex, cover * (ONE_PIXEL * 2), worker->count_ex - x);
	}

	if (worker->render_span && worker->num_gray_spans > 0)
		worker->render_span(worker->span_y, worker->num_gray_spans, worker->gray_spans, worker->render_span_data);
}


static int gray_convert_glyph_inner(PWorker worker)
{
	volatile int error = 0;

	error = FT_Outline_Decompose(&(worker->outline), worker);
	if (error) {return error;}
	error = gray_record_cell(worker);

	return error;
}

static int gray_convert_glyph(PWorker worker)
{
    TBand            bands[40];
    TBand* volatile  band;
    int volatile     n, num_bands;
    TPos volatile    min, max, max_y;
	FT_BBox* clip;

	/* Set up state in the raster object */
	gray_compute_cbox(worker);

	/* clip to target bitmap, exit if nothing to do */
	clip = &worker->clip_box;

	if (worker->max_ex <= clip->xMin || worker->min_ex >= clip->xMax || worker->max_ey <= clip->yMin || worker->min_ey >= clip->yMax)
		return 0;

	if (worker->min_ex < clip->xMin)
		worker->min_ex = clip->xMin;
	if (worker->min_ey < clip->yMin)
		worker->min_ey = clip->yMin;

	if (worker->max_ex > clip->xMax)
		worker->max_ex = clip->xMax;
	if (worker->max_ey > clip->yMax)
		worker->max_ey = clip->yMax;

	worker->count_ex = worker->max_ex - worker->min_ex;
	worker->count_ey = worker->max_ey - worker->min_ey;

	/* set up vertical bands */
	num_bands = (int)((worker->max_ey - worker->min_ey) / worker->band_size);
	if (num_bands == 0)
		num_bands = 1;
	if (num_bands >= 39)
		num_bands = 39;

	worker->band_shoot = 0;

	min = worker->min_ey;
	max_y = worker->max_ey;

	for (n = 0; n < num_bands; n++, min = max)
	{
		max = min + worker->band_size;
		if (n == num_bands - 1 || max > max_y)
			max = max_y;

		bands[0].min = min;
		bands[0].max = max;
		band = bands;

		while (band >= bands)
		{
			TPos bottom, top, middle;
			int error = 0;

			{
				PCell cells_max;
				int yindex;
				long cell_start, cell_end, cell_mod;

				worker->ycells = (PCell*)worker->buffer;
				worker->ycount = band->max - band->min;

				cell_start = sizeof(PCell) * worker->ycount;
				cell_mod = cell_start % sizeof(TCell);
				if (cell_mod > 0)
					cell_start += sizeof(TCell) - cell_mod;

				cell_end = worker->buffer_size;
				cell_end -= cell_end % sizeof(TCell);

				cells_max = (PCell)((char*)worker->buffer + cell_end);
				worker->cells = (PCell)((char*)worker->buffer + cell_start);
				if (worker->cells >= cells_max)
					goto ReduceBands;

				worker->max_cells = cells_max - worker->cells;
				if (worker->max_cells < 2)
					goto ReduceBands;

				for (yindex = 0; yindex < worker->ycount; yindex++)
					worker->ycells[yindex] = NULL;
			}

			worker->num_cells = 0;
			worker->invalid = 1;
			worker->min_ey = band->min;
			worker->max_ey = band->max;
			worker->count_ey = band->max - band->min;

			error = gray_convert_glyph_inner(worker);

			if (!error)
			{
				gray_sweep(worker);
				band--;
				continue;
			}
			else if (error != ErrRaster_Memory_Overflow)
				return error;

ReduceBands:
			/* render pool overflow; we will reduce the render band by half */
			bottom = band->min;
			top = band->max;
			middle = bottom + ((top - bottom) >> 1);

			/* This is too complex for a single scanline; there must */
			/* be some problems.                                     */
			if (middle == bottom)
			{
				return 1;
			}

			if (bottom - top >= worker->band_size)
				worker->band_shoot++;

			band[1].min = bottom;
			band[1].max = middle;
			band[0].min = middle;
			band[0].max = top;
			band++;
		}
	}

	if (worker->band_shoot > 8 && worker->band_size > 16)
		worker->band_size = worker->band_size / 2;

	return 0;
}

static void gray_render_span(int y, int count, const FT_Span* spans, PWorker worker)
{
	unsigned char* p;
	FT_Bitmap* map = &worker->target;

	/* first of all, compute the scanline offset */
	p = (unsigned char*)map->buffer - y * map->pitch;
	if (map->pitch >= 0)
		p += (unsigned)((map->rows - 1) * map->pitch);

	for (; count > 0; count--, spans++)
	{
		unsigned char coverage = spans->coverage;

		if (coverage)
		{
			/* For small-spans it is faster to do it by ourselves than
			 * calling `memset'.  This is mainly due to the cost of the
			 * function call.
			 */
			if (spans->len >= 8)
				memset(p + spans->x, (unsigned char)coverage, spans->len);
			else
			{
				unsigned char* q = p + spans->x;

				switch (spans->len)
				{
					case 7:
						*q++ = (unsigned char)coverage;
						//no break
					case 6:
						*q++ = (unsigned char)coverage;
						//no break
					case 5:
						*q++ = (unsigned char)coverage;
						//no break
					case 4:
						*q++ = (unsigned char)coverage;
						//no break
					case 3:
						*q++ = (unsigned char)coverage;
						//no break
					case 2:
						*q++ = (unsigned char)coverage;
						//no break
					case 1:
						*q = (unsigned char)coverage;
						//no break
					default:
						;
						//no break
				}
			}
		}
	}
}

static void gray_init_cells(PWorker worker, void* buffer, unsigned int byte_size)
{
	worker->buffer = buffer;
	worker->buffer_size = byte_size;

	worker->ycells = (PCell*)buffer;
	worker->cells = NULL;
	worker->max_cells = 0;
	worker->num_cells = 0;
	worker->area = 0;
	worker->cover = 0;
	worker->invalid = 1;
}


#define RENDER_BUFFER_SIZE	(1024 * 6)
unsigned char render_buffer[RENDER_BUFFER_SIZE];

unsigned char* GetRasterBuffer(void)
{
	return render_buffer;
}

unsigned int GetRasterBufferSize(void)
{
	return RENDER_BUFFER_SIZE;
}

static int gray_raster_render(FT_Outline* outline, FT_Bitmap* target_map)
{
	TWorker worker;

	/* return immediately if the outline is empty */
	if (outline->n_points == 0 || outline->n_contours <= 0)
		return 0;

	if (!outline->contours || !outline->points)
		return -1;

	if (outline->n_points != outline->contours[outline->n_contours - 1] + 1)
		return -1;

	/* we must have a target bitmap */
	if (!target_map)
		return -2;

	/* nothing to do */
	if (!target_map->width || !target_map->rows)
		return 0;

	if (!target_map->buffer)
		return -2;


	memset(&worker, 0, sizeof(TWorker));
	/* compute clip box from target pixmap */
	worker.clip_box.xMin = 0;
	worker.clip_box.yMin = 0;
	worker.clip_box.xMax = target_map->width;
	worker.clip_box.yMax = target_map->rows;

	gray_init_cells(&worker, GetRasterBuffer(), GetRasterBufferSize());

	worker.outline = *outline;
	worker.num_cells = 0;
	worker.invalid = 1;
	worker.num_gray_spans = 0;
	worker.band_size = (int)( GetRasterBufferSize() / ( sizeof ( TCell ) * 8 ) );

	worker.target = *target_map;
	worker.render_span = (FT_SpanFunc)gray_render_span;
	worker.render_span_data = &worker;

	return gray_convert_glyph(&worker);
}

static void FT_Outline_Get_CBox(const FT_Outline* outline, FT_BBox *acbox)
{
	FT_Pos xMin, yMin, xMax, yMax;

	if (outline && acbox)
	{
		if (outline->n_points == 0)
		{
			xMin = 0;
			yMin = 0;
			xMax = 0;
			yMax = 0;
		}
		else
		{
			FT_Vector* vec = outline->points;
			FT_Vector* limit = vec + outline->n_points;

			xMin = xMax = vec->x;
			yMin = yMax = vec->y;
			vec++;

			for (; vec < limit; vec++)
			{
				FT_Pos x, y;

				x = vec->x;
				if (x < xMin)
					xMin = x;
				if (x > xMax)
					xMax = x;

				y = vec->y;
				if (y < yMin)
					yMin = y;
				if (y > yMax)
					yMax = y;
			}
		}
		acbox->xMin = xMin;
		acbox->xMax = xMax;
		acbox->yMin = yMin;
		acbox->yMax = yMax;
	}
}

static void FT_Outline_Translate(const FT_Outline* outline, FT_Pos xOffset, FT_Pos yOffset)
{
	short n;
	FT_Vector* vec;

	if (!outline)
		return;

	vec = outline->points;

	for (n = 0; n < outline->n_points; n++)
	{
		vec->x += xOffset;
		vec->y += yOffset;
		vec++;
	}
}


static FT_Vector font_points[FONT_MAX_POINT];
static char font_tags[FONT_MAX_TAG];
static short font_contours[FONT_MAX_CONTOUR];

static BOOL GsOutlineToFtOutline(FT_Outline* outline, GS_TT_OUTLINE* pstOutline, int x_percent, int y_percent)
{
	int i;
	int contours_index;

	memset(outline, 0, sizeof(FT_Outline));
	if (pstOutline->ucPointsNum > FONT_MAX_POINT || pstOutline->ucPointsNum > FONT_MAX_TAG || pstOutline->ucContoursNum > FONT_MAX_CONTOUR)
	{
		return FALSE;
	}
	outline->points = font_points;
	for (i = 0; i < pstOutline->ucPointsNum; ++i)
	{
		outline->points[i].x = (pstOutline->astPoints[i].x << 4) * x_percent / 100;
		outline->points[i].y = (pstOutline->astPoints[i].y << 4) * y_percent / 100;
	}
	outline->tags = font_tags;
	for (i = 0; i < pstOutline->ucPointsNum; ++i)
	{

		outline->tags[i] = (pstOutline->acTags[i / 8] & (1 << (7 - (i % 8)))) >> (7 - (i % 8)) ? FT_CURVE_TAG_ON : FT_CURVE_TAG_CONIC;
	}
	outline->contours = font_contours;
	contours_index = 0;
	for (i = 0; i < pstOutline->ucContoursNum; ++i)
	{
		if (pstOutline->acContours[i] & 0x80)
		{
			outline->contours[i] = (((pstOutline->acContours[contours_index] & 0x7f) << 7) | (pstOutline->acContours[contours_index + 1] & 0x7f));
			contours_index += 2;
		}
		else
		{
			outline->contours[i] = pstOutline->acContours[contours_index];
			contours_index++;
		}
	}
	outline->n_contours = pstOutline->ucContoursNum;
	outline->n_points = pstOutline->ucPointsNum;
	outline->flags = 0;

	return TRUE;
}


static int MutexRender = 0;//GUI_INVALID_MUTEX_HANDLE;

int RenderGlyph(GS_TT_OUTLINE *pstOutline, GS_TT_BITMAP *pstBitmap, int x_percent, int y_percent)
{
	FT_Outline outline;
	FT_BBox cbox;
	FT_Pos x_shift, y_shift;
	FT_Bitmap bitmap;

	if (MutexRender == GUI_INVALID_MUTEX_HANDLE)
	{
		gui_mutex_init(&MutexRender, ""/*"Render"*/);
	}

	if (MutexRender == GUI_INVALID_MUTEX_HANDLE)
	{
		return 0;
	}

	gui_mutex_lock(&MutexRender, GUI_MUTEX_WAIT_FOREVER); //add to sync with media subtitle--eagle

	if (!GsOutlineToFtOutline(&outline, pstOutline, x_percent, y_percent))
	{
		goto EXIT;
	}

	/* compute the control box, and grid fit it */
	FT_Outline_Get_CBox(&outline, &cbox);
	cbox.xMin = FT_PIX_FLOOR( cbox.xMin );
	cbox.yMin = FT_PIX_FLOOR( cbox.yMin );
	cbox.xMax = FT_PIX_CEIL( cbox.xMax );
	cbox.yMax = FT_PIX_CEIL( cbox.yMax );

	pstBitmap->iWidth = (FT_UInt)((cbox.xMax - cbox.xMin) >> 6);
	pstBitmap->iHeight = (FT_UInt)((cbox.yMax - cbox.yMin) >> 6);

	x_shift = (FT_Int)cbox.xMin;
	y_shift = (FT_Int)cbox.yMin;
	pstBitmap->iLeft = (FT_Int)(cbox.xMin >> 6);
	pstBitmap->iTop = (FT_Int)(cbox.yMax >> 6);

	bitmap.pixel_mode = 2; //8 bits gray
	bitmap.num_grays = 256;
	bitmap.width = pstBitmap->iWidth;
	bitmap.rows = pstBitmap->iHeight;
	bitmap.pitch = pstBitmap->iWidth;
	bitmap.buffer = pstBitmap->pucBuffer;

	/* translate outline to render it into the bitmap */
	FT_Outline_Translate(&outline, -x_shift, -y_shift);

	/* render outline into bitmap */
	gray_raster_render(&outline, &bitmap);

EXIT:
	gui_mutex_unlock(&MutexRender);

	return 0;
}

int get_glyph_cbox(GS_TT_OUTLINE *pstOutline, GS_TT_BITMAP *pstBitmap, int x_percent, int y_percent)
{
	FT_Outline outline;
	FT_BBox cbox;

	if (!GsOutlineToFtOutline(&outline, pstOutline, x_percent, y_percent))
	{
		return 0;
	}

	FT_Outline_Get_CBox(&outline, &cbox);
	cbox.xMin = FT_PIX_FLOOR( cbox.xMin );
	cbox.yMin = FT_PIX_FLOOR( cbox.yMin );
	cbox.xMax = FT_PIX_CEIL( cbox.xMax );
	cbox.yMax = FT_PIX_CEIL( cbox.yMax );

	pstBitmap->iWidth = (FT_UInt)((cbox.xMax - cbox.xMin) >> 6);
	pstBitmap->iHeight = (FT_UInt)((cbox.yMax - cbox.yMin) >> 6);

	pstBitmap->iLeft = (FT_Int)(cbox.xMin >> 6);
	pstBitmap->iTop = (FT_Int)(cbox.yMax >> 6);

	return 0;
}
