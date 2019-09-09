//#include "usif.h"

#if 0//ENABLE_KHMER_LANGUAGE
#include "khmer_transform.h"
#include "usifext.h"
#include "font_metrics.h"

static int glyph_info_get_general_category (U16 unicode_props)
{
	return (unicode_props & UPROPS_MASK_GEN_CAT);
}

static BOOL glyph_info_ligated (U16 glyph_props)
{
  return (glyph_props & GLYPH_PROPS_LIGATED);
}

static BOOL glyph_info_is_default_ignorable (U16 unicode_props, U16 glyph_props)
{
  return (unicode_props & UPROPS_MASK_IGNORABLE) && !glyph_info_ligated (glyph_props);
}

static BOOL glyph_info_is_mark (U16 glyph_props)
{
  return (glyph_props & GLYPH_PROPS_MARK);
}

static void set_khmer_text_glyph_props(FONT_GLYPH_MAP *font_glyph_map)//substitute_start
{
	int count = 0;
	int props_temp = 0;

	for (count = 0; count < font_glyph_map->font_glyph_num; count++)
	{
		font_glyph_map->font_fun_glyph[count].glyph_props = font_glyph_map->font_fun_glyph[count].glyph;
		props_temp = get_char_glyph_props(font_glyph_map->font_fun_glyph[count], font_glyph_map->font_fun_glyph[count].glyph);

		if (!props_temp)
		{
			  props_temp = (glyph_info_get_general_category (font_glyph_map->font_fun_glyph[count].unicode_props) != UNICODE_GENERAL_CATEGORY_NON_SPACING_MARK ||
					   glyph_info_is_default_ignorable (font_glyph_map->font_fun_glyph[count].unicode_props, font_glyph_map->font_fun_glyph[count].glyph_props)) ?
					   GLYPH_PROPS_BASE_GLYPH : GLYPH_PROPS_MARK;
		}
		font_glyph_map->font_fun_glyph[count].glyph_props = props_temp;
		font_glyph_map->font_fun_glyph[count].lig_props = 0;
		font_glyph_map->font_fun_glyph[count].syllable = 0;
	}
}

static void khmer_lookup_set_glyph_props (FONT_GLYPH_MAP *font_glyph_map, int cur_select, U16 glyph_index,  unsigned int class_guess, BOOL ligature, BOOL component)//_set_glyph_props
{
	U16 glyph_props_src = font_glyph_map->font_fun_glyph[cur_select].glyph_props;
	U16 add_in = glyph_props_src & GLYPH_PROPS_PRESERVE;

	add_in |= GLYPH_PROPS_SUBSTITUTED;
	if (ligature)
	{
		add_in |= GLYPH_PROPS_LIGATED;
		add_in &= ~GLYPH_PROPS_MULTIPLIED;
	}
	if (component)
	{
		add_in |= GLYPH_PROPS_MULTIPLIED;
	}

	if (1) //has glyph classdef in gdef
	{
		font_glyph_map->font_fun_glyph[cur_select].glyph_props = add_in | get_char_glyph_props (font_glyph_map->font_fun_glyph[cur_select], glyph_index);
	}
	else if (class_guess)
	{
		font_glyph_map->font_fun_glyph[cur_select].glyph_props = add_in | class_guess;
	}
}

static BOOL is_pref_lookup(int lookup_index)
{
	BOOL ret = FALSE;

	if (lookup_index == 1)
	{
		ret = TRUE;
	}

	return ret;
}

static BOOL is_blwm_lookup(int lookup_index, int seq)
{
	BOOL ret = FALSE;

	if (lookup_index == 0 && seq != 0)
	{
		ret = TRUE;
	}

	return ret;
}

static U32 get_indic_unicode_category_type(U16 unicode)
{
	U32 category_type = 3840;

	if ((unicode >= 0x17c1 && unicode <= 0x17c3))
	{
		category_type = 775;
	}
	else if (unicode == 0x17cc)
	{
		category_type = 1539;
	}
	else if ((unicode >= 0x17b7 && unicode <= 0x17ba) || (unicode >= 0x17cd && unicode <= 0x17d1) || (unicode == 0x17be) || (unicode == 0x17cb)
			 || (unicode == 0x17d3))
	{
		category_type = 1543;
	}
	else if (unicode == 0x17c6)
	{
		category_type = 1544;
	}
	else if ((unicode == 0x17c9) || (unicode == 0x17ca))
	{
		category_type = 1549;
	}
	else if ((unicode >= 0x17bb && unicode <= 0x17bd))
	{
		category_type = 2055;
	}
	else if ((unicode >= 0x17bf && unicode <= 0x17c0) || (unicode >= 0x17c4 && unicode <= 0x17c5) || (unicode == 0x17c8) || (unicode == 0x17b6))
	{
		category_type = 2823;
	}
	else if (unicode == 0x17c7)
	{
		category_type = 2824;
	}
	else if (unicode >= 0x1780 && unicode <= 0x17a2)
	{
		category_type = 3841;
	}
	else if (unicode >= 0x17a3 && unicode <= 0x17b3)
	{
		category_type = 3842;
	}
	else if (unicode == 0x200c)
	{
		category_type = 3845;
	}
	else if (unicode == 0x200d)
	{
		category_type = 3846;
	}
	else if ((unicode >= 0x30 && unicode <= 0x39) || (unicode >= 0x17e0 && unicode <= 0x17e9) || (unicode == 0x25cc) || (unicode == 0x2d))
	{
		category_type = 3851;
	}
	else if (unicode == 0x17d2)
	{
		category_type = 3854;
	}
	else if (unicode == 0x17dc)
	{
		category_type = 3858;
	}

	return category_type;
}

static U8 matra_position (U32 unicode, U8 pos)
{
	switch (pos)
	{
		case POS_PRE_C:		return MATRA_POS_LEFT (unicode);
		case POS_POST_C:	return MATRA_POS_RIGHT (unicode);
		case POS_ABOVE_C:	return MATRA_POS_TOP (unicode);
		case POS_BELOW_C:	return MATRA_POS_BOTTOM (unicode);
	};

	return pos;
}

void set_khmer_unicode_cat_pos(U16 unicode, FONT_GLYPH *font_glyph)
{
	U32 type = get_indic_unicode_category_type(unicode);
	U8 cat = (type & 0x7Fu);
	U8 pos = (type >> 8);

	switch (unicode)
	{
		case 0x17dd:
			cat = OT_M;
			pos = POS_ABOVE_C;
			break;
		case 0x17c6:
			cat = OT_N;
			break;
		case 0x25cc:
			cat = OT_DOTTEDCIRCLE;
			break;
		default:
			break;
	}

	if ((FLAG_SAFE (cat) & CONSONANT_FLAGS))
	{
		pos = POS_BASE_C;
		if (unicode == 0x179a)
		{
			cat = OT_Ra;
		}
	}
	else if (cat == OT_M)
	{
		pos = matra_position (unicode, pos);
	}
	else if ((FLAG_SAFE (cat) & (FLAG (OT_SM) | FLAG (OT_VD) | FLAG (OT_A) | FLAG (OT_Symbol))))
	{
		pos = POS_SMVD;
	}

	font_glyph->cat = cat;
	font_glyph->pos = pos;
}

static unsigned int last = 0;
static unsigned int p = 0;
static unsigned int syllable_serial = 1;
static void found_syllable(FONT_GLYPH_MAP *font_glyph_map, U8 syllable_type)
{
	int i = 0;

    for (i = last; i < p+1; i++)
    {
    	font_glyph_map->font_fun_glyph[i].syllable = (syllable_serial << 4) | syllable_type;
    }
    last = p+1;
    syllable_serial++;
    if (syllable_serial == 16)
    {
    	syllable_serial = 1;
    }
}

static void setup_syllables(FONT_GLYPH_MAP *font_glyph_map)
{
	unsigned int pe, eof, ts = 0, te = 0, act = 0;
	int cs = indic_syllable_machine_start;
	int _slen;
	int _trans;
	const unsigned char *_keys;
	const short *_inds;

	last = 0;
	p = 0;
	syllable_serial = 1;
	pe = eof = font_glyph_map->font_glyph_num;
	if ( p == pe )
	{
		goto _test_eof;
	}

_resume:
	switch ( _indic_syllable_machine_from_state_actions[cs] )
	{
	case 11:
			ts = p;
			break;
	}

	_keys = _indic_syllable_machine_trans_keys + (cs<<1);
	_inds = _indic_syllable_machine_indicies + _indic_syllable_machine_index_offsets[cs];
	_slen = _indic_syllable_machine_key_spans[cs];
	_trans = _inds[ _slen > 0 && _keys[0] <=( font_glyph_map->font_fun_glyph[p].cat) &&
		(font_glyph_map->font_fun_glyph[p].cat) <= _keys[1] ?
		( font_glyph_map->font_fun_glyph[p].cat) - _keys[0] : _slen ];

_eof_trans:
	cs = _indic_syllable_machine_trans_targs[_trans];

	if ( _indic_syllable_machine_trans_actions[_trans] == 0 )
		goto _again;
	switch ( _indic_syllable_machine_trans_actions[_trans] )
	{
		case 2:
			te = p+1;
			break;
		case 15:
			te = p+1;
			found_syllable (font_glyph_map, consonant_syllable);
			break;
		case 17:
			te = p+1;
			found_syllable (font_glyph_map, vowel_syllable);
			break;
		case 22:
			te = p+1;
			found_syllable (font_glyph_map, standalone_cluster);
			break;
		case 24:
			te = p+1;
			found_syllable (font_glyph_map, symbol_cluster);
			break;
		case 19:
			te = p+1;
			found_syllable (font_glyph_map, broken_cluster);
			break;
		case 12:
			te = p+1;
			found_syllable (font_glyph_map, non_indic_cluster);
			break;
		case 14:
			te = p;
			p--;
			found_syllable (font_glyph_map, consonant_syllable);
			break;
		case 16:
			te = p;
			p--;
			found_syllable (font_glyph_map, vowel_syllable);
			break;
		case 21:
			te = p;
			p--;
			found_syllable (font_glyph_map, standalone_cluster);
			break;
		case 23:
			te = p;
			p--;
			found_syllable (font_glyph_map, symbol_cluster);
			break;
		case 18:
			te = p;
			p--;
			found_syllable (font_glyph_map, broken_cluster);
			break;
		case 20:
			te = p;
			p--;
			found_syllable (font_glyph_map, non_indic_cluster);
			break;
		case 1:
			p = ((te))-1;
			found_syllable (font_glyph_map, consonant_syllable);
			break;
		case 3:
			p = ((te))-1;
			found_syllable (font_glyph_map, vowel_syllable);
			break;
		case 7:
			p = ((te))-1;
			found_syllable (font_glyph_map, standalone_cluster);
			break;
		case 9:
			p = ((te))-1;
			found_syllable (font_glyph_map, symbol_cluster);
			break;
		case 4:
			p = ((te))-1;
			found_syllable (font_glyph_map, broken_cluster);
			break;
		case 5:
			switch( act )
			{
			case 1:
				p = ((te))-1;
				found_syllable (font_glyph_map, consonant_syllable);
				break;
			case 5:
				p = ((te))-1;
				found_syllable (font_glyph_map, broken_cluster);
				break;
			case 6:
				p = ((te))-1;
				found_syllable (font_glyph_map, non_indic_cluster);
				break;
			}
			break;
		case 8:
			te = p+1;
			act = 1;
			break;
		case 6:
			te = p+1;
			act = 5;
			break;
		case 13:
			te = p+1;
			act = 6;
			break;
	}

_again:
	switch ( _indic_syllable_machine_to_state_actions[cs] )
	{
		case 10:
			ts = 0;
			break;
	}

	if ( ++p != pe )
	{
		goto _resume;
	}
_test_eof: {}
	if ( p == eof )
	{
		if ( _indic_syllable_machine_eof_trans[cs] > 0 )
		{
			_trans = _indic_syllable_machine_eof_trans[cs] - 1;
			goto _eof_trans;
		}
	}
}

static BOOL is_one_of (FONT_GLYPH_MAP *font_glyph_map, int cur_select, unsigned int flags)
{
	if (glyph_info_ligated (font_glyph_map->font_fun_glyph[cur_select].glyph_props))
	{
		return false;
	}
	return (FLAG_SAFE (font_glyph_map->font_fun_glyph[cur_select].cat) & flags);
}

static BOOL is_consonant (FONT_GLYPH_MAP *font_glyph_map, int cur_select)
{
  return is_one_of(font_glyph_map, cur_select, CONSONANT_FLAGS);
}

static BOOL is_joiner (FONT_GLYPH_MAP *font_glyph_map, int cur_select)
{
  return is_one_of(font_glyph_map, cur_select, JOINER_FLAGS);
}

static BOOL is_halant_or_coeng (FONT_GLYPH_MAP *font_glyph_map, int cur_select)
{
	return  is_one_of (font_glyph_map, cur_select, HALANT_OR_COENG_FLAGS);
}

static BOOL glyph_info_substituted (U16 glyph_props)
{
  return (glyph_props & GLYPH_PROPS_SUBSTITUTED);
}

static BOOL glyph_info_multiplied (U16 glyph_props)
{
  return (glyph_props & GLYPH_PROPS_MULTIPLIED);
}

static BOOL glyph_info_ligated_and_didnt_multiply (U16 glyph_props)
{
  return glyph_info_ligated (glyph_props) && !glyph_info_multiplied (glyph_props);
}

static void hb_stable_sort (FONT_GLYPH_MAP *font_glyph_map, unsigned int start, unsigned int end)
{
	int i = start, j = i;
	FONT_GLYPH font_glyph_temp;

	for (i = start + 1; i < end; i++)
	{
		j = i;
		while ((j > start) && (font_glyph_map->font_fun_glyph[j-1].pos > font_glyph_map->font_fun_glyph[i].pos))
		{
			j--;
		}
		if (i == j)
		{
			continue;
		}

		font_glyph_temp = font_glyph_map->font_fun_glyph[i];
		memmove(&font_glyph_map->font_fun_glyph[j+1], &font_glyph_map->font_fun_glyph[j], (i - j) * sizeof (FONT_GLYPH));
		font_glyph_map->font_fun_glyph[j] = font_glyph_temp;
	}
}

static void reset_glyph_mask(FONT_GLYPH_MAP *font_glyph_map)
{
	int count = 0;

	for (count = 0; count < font_glyph_map->font_glyph_num; count++)
	{
		font_glyph_map->font_fun_glyph[count].mask = 1;
	}
}

static void initial_reordering_consonant_syllable (FONT_GLYPH_MAP *font_glyph_map, unsigned int start, unsigned int end)
{
	unsigned int base = end, i = 0, j = 0;
	U8 last_pos = POS_START;
	unsigned int last = 0;
	unsigned int syllable = 0;
	unsigned int pref_len = 2;

	base = start;
	for (i = base + 1; i < end; i++)
	{
		if (is_consonant (font_glyph_map, i))
		{
			font_glyph_map->font_fun_glyph[i].pos = POS_BELOW_C;
		}
	}

	for (i = start; i < base; i++)
	{
		font_glyph_map->font_fun_glyph[i].pos = MIN (POS_PRE_C, font_glyph_map->font_fun_glyph[i].pos);
	}

	if (base < end)
	{
		font_glyph_map->font_fun_glyph[i].pos = POS_BASE_C;
	}

	for (i = base + 1; i < end; i++)
	{
		if (font_glyph_map->font_fun_glyph[i].cat == OT_M)
		{
			for (j = i + 1; j < end; j++)
			{
				if (is_consonant (font_glyph_map, j))
				{
					font_glyph_map->font_fun_glyph[i].pos = POS_FINAL_C;
					break;
				}
			}
			break;
		}
	}


	for (i = start; i < end; i++)
	{
		if ((FLAG_SAFE(font_glyph_map->font_fun_glyph[i].cat) & (JOINER_FLAGS | FLAG (OT_N) | FLAG (OT_RS) | MEDIAL_FLAGS | HALANT_OR_COENG_FLAGS)))
		{
			font_glyph_map->font_fun_glyph[i].pos = last_pos;
			if ((font_glyph_map->font_fun_glyph[i].cat == OT_H && font_glyph_map->font_fun_glyph[i].pos == POS_PRE_M))
			{
				for (j = i; j > start; j--)
				{
					if (font_glyph_map->font_fun_glyph[j-1].pos != POS_PRE_M)
					{
						font_glyph_map->font_fun_glyph[i].pos = font_glyph_map->font_fun_glyph[j-1].pos;
						break;
					}
				}
			}
		}
		else if (font_glyph_map->font_fun_glyph[i].pos != POS_SMVD)
		{
			last_pos = font_glyph_map->font_fun_glyph[i].pos;
		}
	}

	last = base;
	for (i = base + 1; i < end; i++)
	{
		if (is_consonant (font_glyph_map, i))
		{
			for (j = last + 1; j < i; j++)
			{
				if (font_glyph_map->font_fun_glyph[j].pos < POS_SMVD)
				{
					font_glyph_map->font_fun_glyph[j].pos = font_glyph_map->font_fun_glyph[i].pos;
				}
			}
			last = i;
		}
		else if (font_glyph_map->font_fun_glyph[i].cat == OT_M)
		{
			last = i;
		}
	}

    hb_stable_sort(font_glyph_map, start, end);

    base = end;
	for (i = start; i < end; i++)
	{
		if (font_glyph_map->font_fun_glyph[i].pos  == POS_BASE_C)
		{
			base = i;
			break;
		}
	}

	{
		U32 mask = 0;

		for (i = start; i < end && font_glyph_map->font_fun_glyph[i].pos == POS_RA_TO_BECOME_REPH; i++)
		{
			font_glyph_map->font_fun_glyph[i].mask = 1;
		}

		mask = 0;
		mask |= 4;
		for (i = start; i < base; i++)
		{
			font_glyph_map->font_fun_glyph[i].mask |= mask;
		}

		mask = 0;
		if (base < end)
		{
			font_glyph_map->font_fun_glyph[base].mask |= mask;
		}

		mask = 4 | 2 | 16;
		for (i = base + 1; i < end; i++)
		{
			font_glyph_map->font_fun_glyph[i].mask  |= mask;
		}
	}

	if (8 && base + pref_len < end)
	{
		for (i = base + 1; i + pref_len - 1 < end; i++)
		{
			U16 glyphs[2];
			for (j = 0; j < pref_len; j++)
			{
				glyphs[j] = font_glyph_map->font_fun_glyph[i + j].glyph;
			}
			if (glyphs[0] == 0x245 && glyphs[1] == 0x184)
			{
				for (j = 0; j < pref_len; j++)
				{
					font_glyph_map->font_fun_glyph[i++].mask |= 8;
				}
				break;
			}
		}
	}

	for (i = start + 1; i < end; i++)
	{
		if (is_joiner (font_glyph_map, i))
		{
			BOOL non_joiner = font_glyph_map->font_fun_glyph[i].cat == OT_ZWNJ;
			unsigned int j = i;
			do
			{
				j--;
				if (non_joiner)
				{
					font_glyph_map->font_fun_glyph[j].mask &= 1;
				}

			} while (j > start && !is_consonant (font_glyph_map, j));
		}
	}
}

static void initial_reordering_standalone_cluster (FONT_GLYPH_MAP *font_glyph_map, unsigned int start, unsigned int end)
{
	initial_reordering_consonant_syllable (font_glyph_map, start, end);
}

static void initial_reordering_syllable (FONT_GLYPH_MAP *font_glyph_map, unsigned int start, unsigned int end)
{
	U8 syllable_type = (font_glyph_map->font_fun_glyph[start].syllable & 0x0F);

	switch (syllable_type)
	{
		case vowel_syllable:
		case consonant_syllable:
			initial_reordering_consonant_syllable(font_glyph_map, start, end);
			break;
		case broken_cluster:
		case standalone_cluster:
			initial_reordering_standalone_cluster(font_glyph_map, start, end);
			break;
		default:
			break;
	}
}

static void initial_reordering(FONT_GLYPH_MAP *font_glyph_map)
{
	int count = 0, start = 0, end = 0;
	U8 syllable = font_glyph_map->font_fun_glyph[start].syllable;

//	insert_dotted_circles (font_glyph_map);

	for (count = 1; count < font_glyph_map->font_glyph_num; count++)
	{
		if (syllable == font_glyph_map->font_fun_glyph[count].syllable)
		{
			continue;
		}
		else
		{
			end = count;
			initial_reordering_syllable (font_glyph_map, start, end);
			syllable = font_glyph_map->font_fun_glyph[count].syllable;
			start = end;
		}
	}

	initial_reordering_syllable (font_glyph_map, start, font_glyph_map->font_glyph_num);
}

static void final_reordering_syllable (FONT_GLYPH_MAP *font_glyph_map, unsigned int start, unsigned int end)
{
	unsigned int base,i,j;
	BOOL try_pref = TRUE;

	for (base = start; base < end; base++)
	{
		if (font_glyph_map->font_fun_glyph[base].pos >= POS_BASE_C)
		{
			if (try_pref && base + 1 < end)
			{
				for (i = base + 1; i < end; i++)
				{
					if ((font_glyph_map->font_fun_glyph[i].mask & 8) != 0)
					{
						if (!(glyph_info_substituted (font_glyph_map->font_fun_glyph[i].glyph_props) && glyph_info_ligated_and_didnt_multiply (font_glyph_map->font_fun_glyph[i].glyph_props)))
						{
							base = i;
							while (base < end && is_halant_or_coeng (font_glyph_map, base))
							{
								base++;
							}
							font_glyph_map->font_fun_glyph[base].pos = POS_BASE_C;

							try_pref = false;
						}
						break;
					}
				}
			}

			if (start < base && font_glyph_map->font_fun_glyph[base].pos > POS_BASE_C)
			{
				base--;
			}
			break;
		}
	}
	if (base == end && start < base && is_one_of (font_glyph_map, base - 1, FLAG (OT_ZWJ)))
	{
		base--;
	}
	if (base < end)
	{
		while (start < base && is_one_of (font_glyph_map, base, (FLAG (OT_N) | HALANT_OR_COENG_FLAGS)))
		{
			base--;
		}
	}

	if (start + 1 < end && start < base)
	{
		unsigned int new_pos = base == end ? base - 2 : base - 1;

		while (new_pos > start && !(is_one_of (font_glyph_map, new_pos, (FLAG (OT_M) | HALANT_OR_COENG_FLAGS))))
		{
			new_pos--;
		}

		if (is_halant_or_coeng (font_glyph_map, new_pos) && font_glyph_map->font_fun_glyph[new_pos].pos != POS_PRE_M)
		{
			/* -> If ZWJ or ZWNJ follow this halant, position is moved after it. */
			if (new_pos + 1 < end && is_joiner (font_glyph_map, new_pos + 1))
			{
				new_pos++;
			}
		}
		else
		{
			new_pos = start; /* No move. */
		}

		if (start < new_pos && font_glyph_map->font_fun_glyph[new_pos].pos != POS_PRE_M)
		{
			/* Now go see if there's actually any matras... */
			for (i = new_pos; i > start; i--)
			{
				if (font_glyph_map->font_fun_glyph[i-1].pos == POS_PRE_M)
				{
					unsigned int old_pos = i - 1;
					if (old_pos < base && base <= new_pos) /* Shouldn't actually happen. */
					{
						base--;
					}

					FONT_GLYPH tmp = font_glyph_map->font_fun_glyph[old_pos];
					memmove (&font_glyph_map->font_fun_glyph[old_pos], &font_glyph_map->font_fun_glyph[old_pos + 1], (new_pos - old_pos) * sizeof (FONT_GLYPH));
					font_glyph_map->font_fun_glyph[new_pos] = tmp;
					new_pos--;
				}
			}
		}
		else
		{
			for (i = start; i < base; i++)
			{
				if (font_glyph_map->font_fun_glyph[i].pos == POS_PRE_M)
				{
		//					buffer->merge_clusters (i, MIN (end, base + 1));
					break;
				}
			}
		}
	}


//  printf("+++ func: %s, line: %d +++ try_pref %d\n", __FUNCTION__, __LINE__, try_pref);
//  printf("+++ func: %s, line: %d +++ base %d\n", __FUNCTION__, __LINE__, base);
//  printf("+++ func: %s, line: %d +++ end %d\n", __FUNCTION__, __LINE__, end);
	if (try_pref && base + 1 < end)
	{
		unsigned int pref_len = 2;
		for (i = base + 1; i < end; i++)
		{
			if ((font_glyph_map->font_fun_glyph[i].mask & 8) != 0)
			{
				if (glyph_info_substituted (font_glyph_map->font_fun_glyph[i].glyph_props) && ((pref_len == 1) ^ glyph_info_ligated_and_didnt_multiply (font_glyph_map->font_fun_glyph[i].glyph_props)))
				{
					unsigned int new_pos = base;
					{
						while (new_pos > start && !(is_one_of (font_glyph_map, new_pos - 1, FLAG(OT_M) | HALANT_OR_COENG_FLAGS)))
						{
							new_pos--;
						}

						if (new_pos > start && font_glyph_map->font_fun_glyph[new_pos - 1].cat == OT_M)
						{
							unsigned int old_pos = i;
							for (j = base + 1; j < old_pos; j++)
							{
								if (font_glyph_map->font_fun_glyph[j].cat== OT_M)
								{
									new_pos--;
									break;
								}
							}
						}
					}

					if (new_pos > start && is_halant_or_coeng (font_glyph_map, new_pos - 1))
					{
						/* -> If ZWJ or ZWNJ follow this halant, position is moved after it. */
						if (new_pos < end && is_joiner (font_glyph_map, new_pos))
						{
							new_pos++;
						}
					}

					{
						unsigned int old_pos = i;

						//						buffer->merge_clusters (new_pos, old_pos + 1);
						FONT_GLYPH tmp = font_glyph_map->font_fun_glyph[old_pos];
						memmove (&font_glyph_map->font_fun_glyph[new_pos + 1], &font_glyph_map->font_fun_glyph[new_pos], (old_pos - new_pos) * sizeof (FONT_GLYPH));
						font_glyph_map->font_fun_glyph[new_pos] = tmp;

						if (new_pos <= base && base < old_pos)
						{
							base++;
						}
					}
				}
				break;
			}
		}
	}
}


static void final_reordering (FONT_GLYPH_MAP *font_glyph_map)
{
	int count = 0, start = 0, end = 0;
	U8 syllable = font_glyph_map->font_fun_glyph[start].syllable;

	for (count = 1; count < font_glyph_map->font_glyph_num; count++)
	{
		if (syllable == font_glyph_map->font_fun_glyph[count].syllable)
		{
			continue;
		}
		else
		{
			end = count;
			final_reordering_syllable (font_glyph_map, start, end);
			syllable = font_glyph_map->font_fun_glyph[count].syllable;
			start = end;
		}
	}

	final_reordering_syllable (font_glyph_map, start, font_glyph_map->font_glyph_num);
}

static void khmer_replace_glyph_with_ligture(FONT_GLYPH_MAP *font_glyph_map, int cur_select, U16 dst_glyph, int class_guess, BOOL ligture) //replace_glyph_with_ligature
{
	khmer_lookup_set_glyph_props(font_glyph_map, cur_select, dst_glyph, class_guess, ligture, FALSE); //_set_glyph_props
	font_glyph_map->font_fun_glyph[cur_select].glyph = dst_glyph;
}

static U16 khmer_get_lookup_props(int lookup_index)
{
	U16 lookup_props = 0;

	switch (lookup_index)
	{
		case 18:
		case 43:
			lookup_props = 0x100;
			break;
		case 22:
			lookup_props = 8;
			break;
		default:
			break;
	}

	return lookup_props;
}

static U16 khmer_get_lookup_mask(int lookup_index)
{
	U16 lookup_msk = 1;

	switch (lookup_index)
	{
		case 1:
			lookup_msk = 8;
			break;
		case 2:
		case 3:
		case 4:
		case 5:
		case 25:
		case 27:
			lookup_msk = 4;
			break;
		case 6:
		case 7:
			lookup_msk = 2;
			break;
		case 8:
			lookup_msk = 16;
			break;
		default:
			break;
	}

	return lookup_msk;
}

static BOOL match_properties_mark(U16 glyph_props, U16 match_props)
{
	if (match_props & UseMarkFilteringSet)
	{
		return FALSE;
	}

	if (match_props & MarkAttachmentType)
	{
		return (match_props & MarkAttachmentType) == (glyph_props & MarkAttachmentType);
	}

	return TRUE;
}

static BOOL check_glyph_property (FONT_GLYPH_MAP *font_glyph_map, int cur_select, unsigned int lookup_index)
{
	U16 glyph_props = font_glyph_map->font_fun_glyph[cur_select].glyph_props;
	U16 match_props = khmer_get_lookup_props(lookup_index);

	if (glyph_props & match_props & IgnoreFlags)
	{
		return FALSE;
	}

	if (glyph_props & GLYPH_PROPS_MARK)
	{
		return match_properties_mark (glyph_props, match_props);
	}

	return TRUE;
}

static BOOL check_glyph_syllable (FONT_GLYPH_MAP *font_glyph_map, int cur_select, int dst_select)
{
	U8 cur_glyph_syllable = font_glyph_map->font_fun_glyph[cur_select].syllable;
	U8 dst_glyph_syllable = font_glyph_map->font_fun_glyph[dst_select].syllable;
	BOOL ret = FALSE;

	if (dst_glyph_syllable && (dst_glyph_syllable == cur_glyph_syllable))
	{
		ret = TRUE;
	}

	return ret;
}

static BOOL check_glyph_mask (FONT_GLYPH_MAP *font_glyph_map, int cur_select, int lookup_index)
{
	BOOL ret = FALSE;

	if (khmer_get_lookup_mask(lookup_index) & font_glyph_map->font_fun_glyph[cur_select].mask)
	{
		ret = TRUE;
	}

	return ret;
}

static BOOL check_glyph_info_for_chain (FONT_GLYPH_MAP *font_glyph_map, int cur_select, int dst_select, int lookup_index)
{
	BOOL ret = TRUE;

	if (!(check_glyph_property(font_glyph_map, dst_select, lookup_index) && font_glyph_map->font_fun_glyph[cur_select].mask
			&& check_glyph_syllable(font_glyph_map, cur_select, dst_select)))
	{
		return FALSE;
	}

	return ret;
}

static unsigned int glyph_info_get_lig_id (U8 lig_props)
{
  return lig_props >> 5;
}

static int serial_lig_count = 1;
static U8 allocate_lig_id (void)
{
	U8 lig_id = (serial_lig_count & 0x07);

	serial_lig_count++;

	return lig_id;
}

static BOOL glyph_info_ligated_internal (U8 lig_props)
{
	return (lig_props & IS_LIG_BASE);
}

static unsigned int glyph_info_get_lig_num_comps (U8 lig_props, U16 glyph_props)
{
	if ((glyph_props & GLYPH_PROPS_LIGATURE) && glyph_info_ligated_internal (lig_props))
	{
	  return (lig_props & 0x0F);
	}
	else
	{
	  return 1;
	}
}

static unsigned int glyph_info_get_lig_comp (U8 lig_props)
{
	if (glyph_info_ligated_internal (lig_props))
	{
		return 0;
	}
	else
	{
		return (lig_props & 0x0F);
	}
}

static void glyph_info_set_lig_props_for_ligature (FONT_GLYPH_MAP *font_glyph_map, int cur_select, unsigned int lig_id, unsigned int lig_num_comps)
{
	font_glyph_map->font_fun_glyph[cur_select].lig_props = (lig_id << 5) | IS_LIG_BASE | (lig_num_comps & 0x0F);
}

static void glyph_info_set_lig_props_for_mark (FONT_GLYPH_MAP *font_glyph_map, int cur_select, unsigned int lig_id, unsigned int lig_comp)
{
	font_glyph_map->font_fun_glyph[cur_select].lig_props = (lig_id << 5) | (lig_comp & 0x0F);
}

static void glyph_info_set_general_category (FONT_GLYPH_MAP *font_glyph_map, int cur_select, int gen_cat)
{
	font_glyph_map->font_fun_glyph[cur_select].unicode_props = (unsigned int) gen_cat | (font_glyph_map->font_fun_glyph[cur_select].unicode_props & (0xFF & ~UPROPS_MASK_GEN_CAT));
}

static BOOL match_input (FONT_GLYPH_MAP *font_glyph_map, int cur_select, int lookup_index, int *total_component_count, int count, BOOL *is_mark_ligature)
{
	U8 lig_props = font_glyph_map->font_fun_glyph[cur_select].lig_props;
	U16 glyph_props = font_glyph_map->font_fun_glyph[cur_select].glyph_props;
	U8 dst_lig_props = 0;
	U16 dst_glyph_props = 0;
	unsigned int i = 0;
	unsigned int first_lig_id = glyph_info_get_lig_id (lig_props);
	unsigned int first_lig_comp = glyph_info_get_lig_comp (lig_props);
	unsigned int dst_lig_id = 0;
	unsigned int dst_lig_comp = 0;

	*total_component_count += glyph_info_get_lig_num_comps (lig_props, glyph_props);
	*is_mark_ligature = glyph_info_is_mark(glyph_props);
	for (i = 1; i < count; i++)
	{
		if (!(check_glyph_property(font_glyph_map, cur_select + i, lookup_index) && check_glyph_mask(font_glyph_map, cur_select + i, lookup_index)
				&& check_glyph_syllable(font_glyph_map, cur_select, cur_select + i)))
		{
			return FALSE;
		}

		dst_lig_props = font_glyph_map->font_fun_glyph[cur_select + i].lig_props;
		dst_lig_id = glyph_info_get_lig_id (dst_lig_props);
		dst_lig_comp = glyph_info_get_lig_comp (dst_lig_props);

		if (first_lig_id && first_lig_comp)
		{
			if (first_lig_id != dst_lig_id || first_lig_comp != dst_lig_comp)
			{
				return FALSE;
			}
		}
		else
		{
			if (dst_lig_id && dst_lig_comp && (dst_lig_id != first_lig_id))
			{
				return FALSE;
			}
		}

		dst_glyph_props = font_glyph_map->font_fun_glyph[cur_select + i].glyph_props;
		*total_component_count += glyph_info_get_lig_num_comps (dst_lig_props, dst_glyph_props);
		*is_mark_ligature = *is_mark_ligature && glyph_info_is_mark (dst_glyph_props);
	}

	return TRUE;
}

static BOOL ligate_input (FONT_GLYPH_MAP *font_glyph_map, int cur_select, U16 lig_glyph, int total_component_count, int count, BOOL is_mark_ligature)
{
	U8 lig_props = font_glyph_map->font_fun_glyph[cur_select].lig_props;
	U16 glyph_props = font_glyph_map->font_fun_glyph[cur_select].glyph_props;
	U16 unicode_props = font_glyph_map->font_fun_glyph[cur_select].unicode_props;
	U16 dst_glyph_props = font_glyph_map->font_fun_glyph[cur_select + 1].glyph_props;
	unsigned int klass = is_mark_ligature ? 0 : GLYPH_PROPS_LIGATURE;
	unsigned int lig_id = is_mark_ligature ? 0 : allocate_lig_id ();
	unsigned int last_lig_id = glyph_info_get_lig_id (lig_props);
	unsigned int last_num_components = glyph_info_get_lig_num_comps (lig_props, glyph_props);
	unsigned int components_so_far = last_num_components;
	unsigned int this_comp = 0;
	unsigned int new_lig_comp = 0;
	int i = 0;
	int str_len = font_glyph_map->font_glyph_num;
	int char_info_size = sizeof(FONT_GLYPH);
	int cur_select_src = cur_select;

	if (!is_mark_ligature)
	{
		glyph_info_set_lig_props_for_ligature (font_glyph_map, cur_select, lig_id, total_component_count);
		if (glyph_info_get_general_category (unicode_props) == UNICODE_GENERAL_CATEGORY_NON_SPACING_MARK)
		{
			glyph_info_set_general_category (font_glyph_map, cur_select, UNICODE_GENERAL_CATEGORY_OTHER_LETTER);
		}
	}
	khmer_replace_glyph_with_ligture(font_glyph_map, cur_select, lig_glyph, klass, TRUE);

	cur_select++;
	for (i = 1; i < count; i++)
	{
		lig_props = font_glyph_map->font_fun_glyph[cur_select].lig_props;
		glyph_props = font_glyph_map->font_fun_glyph[cur_select].glyph_props;
		last_lig_id = glyph_info_get_lig_id (lig_props);
		last_num_components = glyph_info_get_lig_num_comps (lig_props, glyph_props);
		components_so_far += last_num_components;
		cur_select++;
	}

	if (!is_mark_ligature && last_lig_id)
	{
		for (i = cur_select; i < font_glyph_map->font_glyph_num; i++)
		{
			lig_props =  font_glyph_map->font_fun_glyph[cur_select].lig_props;
			if (last_lig_id == glyph_info_get_lig_id (lig_props))
			{
				this_comp = glyph_info_get_lig_comp (lig_props);
				if (!this_comp)
				{
					break;
				}
				new_lig_comp = components_so_far - last_num_components + MIN (this_comp, last_num_components);
				glyph_info_set_lig_props_for_mark (font_glyph_map, i, lig_id, new_lig_comp);
			}
			else
			{
				break;
			}
		}
	}

	memmove(font_glyph_map->font_fun_glyph + cur_select_src + 1, font_glyph_map->font_fun_glyph + cur_select_src + count, (str_len - cur_select_src - count) * char_info_size);
	font_glyph_map->font_glyph_num -= (count - 1);

	return TRUE;
}

static BOOL is_khmer_special_char(int index, U16 char_code)
{
	BOOL need_trans = FALSE;

	switch (index)
	{
		case 0:
			if (char_code == 0x120 || char_code == 0x122)
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0xea && char_code <= 0xf4 && ((char_code - 0xea) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x102 && char_code <= 0x111 && ((char_code - 0x102) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x129 && char_code <= 0x133 && ((char_code - 0x129) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x141 && char_code <= 0x15a && ((char_code - 0x141) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x168 && char_code <= 0x177 && ((char_code - 0x168) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x18a && char_code <= 0x194 && ((char_code - 0x18a) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x1ab && char_code <= 0x1ba && ((char_code - 0x1ab) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x1bd && char_code <= 0x1ea && ((char_code - 0x1bd) % 3 == 0))
			{
				need_trans = TRUE;
			}
			break;
		case 1:
			if (char_code == 0x11e || char_code == 0x1a1)
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0xe9 && char_code <= 0xf8 && ((char_code - 0xe9) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x101 && char_code <= 0x115 && ((char_code - 0x101) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x128 && char_code <= 0x137 && ((char_code - 0x128) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x140 && char_code <= 0x15e && ((char_code - 0x140) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x167 && char_code <= 0x17b && ((char_code - 0x167) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x184 && char_code <= 0x198 && ((char_code - 0x184) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x1aa && char_code <= 0x1b9 && ((char_code - 0x1aa) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x1bc && char_code <= 0x1e9 && ((char_code - 0x1bc) % 3 == 0))
			{
				need_trans = TRUE;
			}
			break;
		case 2:
			if ((char_code >= 0x120 && char_code <= 0x123) || (char_code == 0x1a2))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0xea && char_code <= 0xf9 && ((char_code - 0xea) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0xeb && char_code <= 0xf5 && ((char_code - 0xeb) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x102 && char_code <= 0x116 && ((char_code - 0x102) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x103 && char_code <= 0x112 && ((char_code - 0x103) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x129 && char_code <= 0x138 && ((char_code - 0x129) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x12a && char_code <= 0x134 && ((char_code - 0x12a) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x141 && char_code <= 0x15f && ((char_code - 0x141) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x142 && char_code <= 0x15b && ((char_code - 0x142) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x168 && char_code <= 0x17c && ((char_code - 0x168) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x169 && char_code <= 0x178 && ((char_code - 0x169) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x18a && char_code <= 0x199 && ((char_code - 0x18a) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x18b && char_code <= 0x195 && ((char_code - 0x18b) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x1ab && char_code <= 0x1ba && ((char_code - 0x1ab) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x1ac && char_code <= 0x1bb && ((char_code - 0x1ac) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x1bd && char_code <= 0x1ea && ((char_code - 0x1bd) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x1be && char_code <= 0x1eb && ((char_code - 0x1be) % 5 == 0))
			{
				need_trans = TRUE;
			}
			break;
		case 3:
			if ((char_code >= 0x120 && char_code <= 0x123) || (char_code == 0x205) || (char_code == 0x206))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0xea && char_code <= 0xf4 && ((char_code - 0xea) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0xeb && char_code <= 0xf5 && ((char_code - 0xeb) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x102 && char_code <= 0x111 && ((char_code - 0x102) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x103 && char_code <= 0x112 && ((char_code - 0x103) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x129 && char_code <= 0x133 && ((char_code - 0x129) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x12a && char_code <= 0x134 && ((char_code - 0x12a) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x141 && char_code <= 0x15a && ((char_code - 0x141) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x142 && char_code <= 0x15b && ((char_code - 0x142) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x168 && char_code <= 0x177 && ((char_code - 0x168) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x169 && char_code <= 0x178 && ((char_code - 0x169) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x18a && char_code <= 0x194 && ((char_code - 0x18a) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x18b && char_code <= 0x195 && ((char_code - 0x18b) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x1ab && char_code <= 0x1ba && ((char_code - 0x1ab) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x1ac && char_code <= 0x1bb && ((char_code - 0x1ac) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x1bd && char_code <= 0x1ea && ((char_code - 0x1bd) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x1be && char_code <= 0x1eb && ((char_code - 0x1be) % 5 == 0))
			{
				need_trans = TRUE;
			}
			break;
		case 4:
			if (char_code == 0x121 || char_code == 0x123 || char_code == 0x1a3)
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0xeb && char_code <= 0xfa && ((char_code - 0xeb) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x103 && char_code <= 0x117 && ((char_code - 0x103) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x12a && char_code <= 0x139 && ((char_code - 0x12a) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x142 && char_code <= 0x160 && ((char_code - 0x142) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x169 && char_code <= 0x17d && ((char_code - 0x169) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x18b && char_code <= 0x19a && ((char_code - 0x18b) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x1ac && char_code <= 0x1bb && ((char_code - 0x1ac) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x1be && char_code <= 0x1eb && ((char_code - 0x1be) % 3 == 0))
			{
				need_trans = TRUE;
			}
			break;
		case 5:
			if (char_code == 0x120 || char_code == 0x122 || char_code == 0x1a2)
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0xea && char_code <= 0xf9 && ((char_code - 0xea) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x102 && char_code <= 0x116 && ((char_code - 0x102) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x129 && char_code <= 0x138 && ((char_code - 0x129) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x141 && char_code <= 0x15f && ((char_code - 0x141) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x168 && char_code <= 0x17c && ((char_code - 0x168) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x18a && char_code <= 0x199 && ((char_code - 0x18a) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x1ab && char_code <= 0x1ba && ((char_code - 0x1ab) % 5 == 0))
			{
				need_trans = TRUE;
			}
			else if (char_code >= 0x1bd && char_code <= 0x1ea && ((char_code - 0x1bd) % 3 == 0))
			{
				need_trans = TRUE;
			}
			break;
		case 6:
			 if (char_code >= 0xe9 && char_code <= 0xf3 && ((char_code - 0xe9) % 5 == 0))
			 {
				 need_trans = TRUE;
			 }
			 else if (char_code >= 0x101 && char_code <= 0x110 && ((char_code - 0x101) % 5 == 0))
			 {
				 need_trans = TRUE;
			 }
			 else if (char_code >= 0x128 && char_code <= 0x132 && ((char_code - 0x128) % 5 == 0))
			 {
				 need_trans = TRUE;
			 }
			 else if (char_code >= 0x140 && char_code <= 0x159 && ((char_code - 0x140) % 5 == 0))
			 {
				 need_trans = TRUE;
			 }
			 else if (char_code >= 0x167 && char_code <= 0x176 && ((char_code - 0x167) % 5 == 0))
			 {
				 need_trans = TRUE;
			 }
			 else if (char_code >= 0x189 && char_code <= 0x193 && ((char_code - 0x189) % 5 == 0))
			 {
				need_trans = TRUE;
			 }
			 else if (char_code >= 0x1aa && char_code <= 0x1b9 && ((char_code - 0x1aa) % 5 == 0))
			 {
				need_trans = TRUE;
			 }
			 else if (char_code >= 0x1bc && char_code <= 0x1ea && ((char_code - 0x1bc) % 3 == 0))
			 {
				need_trans = TRUE;
			 }
			break;
		case 7:
			if ((char_code >= 0x1f1 && char_code <= 0x1f2) || (char_code >= 0x1f7 && char_code <= 0x1f8)
					 || (char_code >= 0x1fb && char_code <= 0x1fc) || (char_code >= 0x1ff && char_code <= 0x200)
					 || (char_code >= 0x21b && char_code <= 0x21c))
			{
				need_trans = TRUE;
			}
			break;
		case 8:
			if ((char_code >= 0x1ef && char_code <= 0x1f0) || (char_code >= 0x1f5 && char_code <= 0x1f6)
					 || (char_code >= 0x1f9 && char_code <= 0x1fa) || (char_code >= 0x1fd && char_code <= 0x1fe)
					 || (char_code == 0x20b))
			{
				need_trans = TRUE;
			}
			break;
		case 9:
			if (((char_code >= 0x1f0 && char_code <= 0x200) && ((char_code - 0x1f0) % 2 == 0)) || (char_code == 0x21a || char_code == 0x21c)
					||  ((char_code >= 0x220 && char_code <= 0x244) && ((char_code - 0x220) % 2 == 0)))
			{
				need_trans = TRUE;
			}
			break;
		case 10:
			switch (char_code)
			{
				case 0x101:
				case 0x11e:
				case 0x159:
				case 0x15e:
				case 0x176:
				case 0x17b:
				case 0x184:
				case 0x189:
				case 0x18e:
					need_trans = TRUE;
					break;
				default:
					break;
			}
			break;
		case 11:
			switch (char_code)
			{
				case 0xf9:
				case 0x116:
				case 0x138:
				case 0x15f:
				case 0x17c:
				case 0x199:
				case 0x1a2:
					need_trans = TRUE;
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}

	return need_trans;
}


static BOOL get_lookup_ahead_0_transform_state(int type, int lookup_index, FONT_GLYPH_MAP *font_glyph_map, int cur_select)
{
	BOOL need_trans = FALSE;
	U16 lookup_ahead_0 = 0;

	if ((cur_select + 1 >= font_glyph_map->font_glyph_num) || (!check_glyph_info_for_chain(font_glyph_map, cur_select, cur_select + 1, lookup_index)))
	{
		return need_trans;
	}

	lookup_ahead_0 = font_glyph_map->font_fun_glyph[cur_select + 1].glyph;

	switch (type)
	{
		case 0:
			if (is_khmer_special_char(0, lookup_ahead_0))
			{
				need_trans = TRUE;
			}
			break;
		case 1:
			if ((lookup_ahead_0 == 0x1ee))
			{
				need_trans = TRUE;
			}
			break;
		case 2:
			if (lookup_ahead_0 == 0x20a)
			{
				need_trans = TRUE;
			}
			break;
		case 3:
			if (lookup_ahead_0 == 0x23d)
			{
				need_trans = TRUE;
			}
			break;
		case 4:
			if (lookup_ahead_0 == 0xe8)
			{
				need_trans = TRUE;
			}
			break;
		case 5:
			if (is_khmer_special_char(1, lookup_ahead_0))
			{
				need_trans = TRUE;
			}
			break;
		case 6:
			if (is_khmer_special_char(2, lookup_ahead_0))
			{
				need_trans = TRUE;
			}
			break;
		case 7:
			if (is_khmer_special_char(3, lookup_ahead_0))
			{
				need_trans = TRUE;
			}
			break;
		case 8:
			if (lookup_ahead_0 == 0xe7)
			{
				need_trans = TRUE;
			}
			break;
		case 9:
			if (is_khmer_special_char(7, lookup_ahead_0))
			{
				need_trans = TRUE;
			}
			break;
		case 10:
			if (is_khmer_special_char(8, lookup_ahead_0))
			{
				need_trans = TRUE;
			}
			break;
		case 11:
			if ((lookup_ahead_0 >= 0x201 && lookup_ahead_0 <= 0x209) || (lookup_ahead_0 == 0x245))
			{
				need_trans = TRUE;
			}
			break;
		default:
			break;
	}

	return need_trans;
}

static BOOL get_lookup_ahead_1_transform_state(int type, int lookup_index, FONT_GLYPH_MAP *font_glyph_map, int cur_select)
{
	BOOL need_trans = FALSE;
	U16 lookup_ahead_1 = 0;

	if ((cur_select + 2 >= font_glyph_map->font_glyph_num) || (!check_glyph_info_for_chain(font_glyph_map, cur_select + 1, cur_select + 2, lookup_index)))
	{
		return need_trans;
	}

	lookup_ahead_1 = font_glyph_map->font_fun_glyph[cur_select + 2].glyph;

	switch (type)
	{
		case 0:
			if (lookup_ahead_1 == 0x219)
			{
				need_trans = TRUE;
			}
			break;
		case 1:
			if (lookup_ahead_1 == 0x1ef)
			{
				need_trans = TRUE;
			}
			break;
		case 2:
			if (lookup_ahead_1 == 0x1f5)
			{
				need_trans = TRUE;
			}
			break;
		case 3:
			if (lookup_ahead_1 == 0x1f9)
			{
				need_trans = TRUE;
			}
			break;
		case 4:
			if (lookup_ahead_1 == 0x1fd)
			{
				need_trans = TRUE;
			}
			break;
		case 5:
			if (is_khmer_special_char(0, lookup_ahead_1))
			{
				need_trans = TRUE;
			}
			break;
		case 6:
			if (is_khmer_special_char(11, lookup_ahead_1))
			{
				need_trans = TRUE;
			}
			break;
		case 7:
			if (is_khmer_special_char(8, lookup_ahead_1))
			{
				need_trans = TRUE;
			}
			break;
		default:
			break;
	}

	return need_trans;
}

static BOOL get_back_track_0_transform_state(int type, int lookup_index, FONT_GLYPH_MAP *font_glyph_map, int cur_select)
{
	BOOL need_trans = FALSE;
	U16 back_track_0 = 0;

	if ((cur_select - 1 < 0) || (!check_glyph_info_for_chain(font_glyph_map, cur_select, cur_select - 1, lookup_index)))
	{
		return need_trans;
	}

	back_track_0 = font_glyph_map->font_fun_glyph[cur_select - 1].glyph;
	switch (type)
	{
		case 0:
			if (is_khmer_special_char(10, back_track_0))
			{
				need_trans = TRUE;
			}
			break;
		case 1:
			if (back_track_0 == 0x1a1 || back_track_0 == 0x1aa || back_track_0 == 0x1b4)
			{
				need_trans = TRUE;
			}
			break;
		case 2:
			if (back_track_0 == 0x1a1)
			{
				need_trans = TRUE;
			}
			break;
		case 3:
			if (back_track_0 == 0x184 || back_track_0 == 0x18e || back_track_0 == 0x220 || back_track_0 == 0x224)
			{
				need_trans = TRUE;
			}
			break;
		case 4:
			if (back_track_0 == 0xe7)
			{
				need_trans = TRUE;
			}
			break;
		case 5:
			if (is_khmer_special_char(0, back_track_0))
			{
				need_trans = TRUE;
			}
			break;
		case 6:
			if (back_track_0 == 0x201)
			{
				need_trans = TRUE;
			}
			break;
		case 7:
			if (is_khmer_special_char(9, back_track_0))
			{
				need_trans = TRUE;
			}
			break;
		case 8:
			if ((back_track_0 >= 0x201 && back_track_0 <= 0x209) || (back_track_0 == 0x245))
			{
				need_trans = TRUE;
			}
			break;
		case 9:
			if (is_khmer_special_char(1, back_track_0))
			{
				need_trans = TRUE;
			}
			break;
		case 10:
			if ((back_track_0 == 0x21f) || (back_track_0 == 0x223))
			{
				need_trans = TRUE;
			}
			break;
		case 11:
			if (is_khmer_special_char(11, back_track_0))
			{
				need_trans = TRUE;
			}
			break;
		case 12:
			if (back_track_0 == 0x11e)
			{
				need_trans = TRUE;
			}
			break;
		case 13:
			if (back_track_0 == 0x1af)
			{
				need_trans = TRUE;
			}
			break;
		case 14:
			if (is_khmer_special_char(4, back_track_0))
			{
				need_trans = TRUE;
			}
			break;
		case 15:
			if ((back_track_0 >= 0x21f && back_track_0 <= 0x220) || (back_track_0 >= 0x223 && back_track_0 <= 0x224))
			{
				need_trans = TRUE;
			}
			break;
		case 16:
			if (back_track_0 == 0xe8)
			{
				need_trans = TRUE;
			}
			break;
		case 17:
			if (back_track_0 == 0x229)
			{
				need_trans = TRUE;
			}
			break;
		case 18:
			if (is_khmer_special_char(8, back_track_0) || (back_track_0 >= 0x21f && back_track_0 <= 0x220) || (back_track_0 >= 0x223 && back_track_0 <= 0x224) || (back_track_0 == 0x229))
			{
				need_trans = TRUE;
			}
			break;
		case 19:
			if (is_khmer_special_char(7, back_track_0))
			{
				need_trans = TRUE;
			}
			break;
		case 20:
			if (is_khmer_special_char(5, back_track_0))
			{
				need_trans = TRUE;
			}
			break;
		default:
			break;
	}

	return need_trans;
}

static BOOL get_back_track_1_transform_state(int type, int lookup_index, FONT_GLYPH_MAP *font_glyph_map, int cur_select)
{
	BOOL need_trans = FALSE;
	U16 back_track_1 = 0;

	if ((cur_select - 2 < 0) || (!check_glyph_info_for_chain(font_glyph_map, cur_select - 1, cur_select - 2, lookup_index)))
	{
		return need_trans;
	}

	back_track_1 = font_glyph_map->font_fun_glyph[cur_select - 2].glyph;
	switch (type)
	{
		case 0:
			if (back_track_1 == 0x220 || back_track_1 == 0x224)
			{
				need_trans = TRUE;
			}
			break;
		case 1:
			if (back_track_1 == 0x184 || back_track_1 == 0x18e)
			{
				need_trans = TRUE;
			}
			break;
		case 2:
			if (is_khmer_special_char(0, back_track_1))
			{
				need_trans = TRUE;
			}
			break;
		case 3:
			if (back_track_1 == 0x185)
			{
				need_trans = TRUE;
			}
			break;
		case 4:
			if (is_khmer_special_char(11, back_track_1))
			{
				need_trans = TRUE;
			}
			break;
		case 5:
			if (back_track_1 == 0x11e)
			{
				need_trans = TRUE;
			}
			break;
		case 6:
			if (is_khmer_special_char(1, back_track_1))
			{
				need_trans = TRUE;
			}
			break;
		case 7:
			if (back_track_1 == 0x21f || back_track_1 == 0x223)
			{
				need_trans = TRUE;
			}
			break;
		case 8:
			if (is_khmer_special_char(10, back_track_1))
			{
				need_trans = TRUE;
			}
			break;
		default:
			break;
	}

	return need_trans;
}

static BOOL get_back_track_2_transform_state(int type, int lookup_index, FONT_GLYPH_MAP *font_glyph_map, int cur_select)
{
	BOOL need_trans = FALSE;
	U16 back_track_2 = 0;

	if ((cur_select - 3 < 0) || (!check_glyph_info_for_chain(font_glyph_map, cur_select - 2, cur_select - 3, lookup_index)))
	{
		return need_trans;
	}

	back_track_2 = font_glyph_map->font_fun_glyph[cur_select - 3].glyph;
	switch (type)
	{
		case 0:
			if (back_track_2 == 0xe9)
			{
				need_trans = TRUE;
			}
			break;
		case 1:
			if (back_track_2 == 0x186)
			{
				need_trans = TRUE;
			}
			break;
		default:
			break;
	}

	return need_trans;
}

static void khmer_text_transform_for_chain_context_lookup(int lookup_index, int cur_select, FONT_GLYPH_MAP *font_glyph_map)
{
	U16 cur_char_code = font_glyph_map->font_fun_glyph[cur_select].glyph;
	U16 lookup_char_code_0 = 0;
	U16 trans_char_code;
	int str_len = font_glyph_map->font_glyph_num;
	BOOL need_trans = FALSE;
	int temp_count = 0;
	U16 temp_char_code = 0;
	U16 dst_glyph = 0;
	int char_info_size = sizeof(FONT_GLYPH);
	int class_guess = 0;
	int total_component_count = 0;
	int component_count = 2;
	BOOL is_mark_ligature = FALSE;

	switch (lookup_index)
	{
		case 30:
			if (cur_char_code == 0x21f)
			{
				dst_glyph = 0x201;
				khmer_replace_glyph_with_ligture(font_glyph_map, cur_select, dst_glyph, 0, FALSE);
			}
			break;
		case 31:
			if (cur_char_code == 0x223)
			{
				dst_glyph = 0x201;
				khmer_replace_glyph_with_ligture(font_glyph_map, cur_select, dst_glyph, 0, FALSE);
			}
			break;
		case 32:
		case 48:
			if ((cur_select + 1) >= str_len)
			{
				break;
			}

			lookup_char_code_0 = font_glyph_map->font_fun_glyph[cur_select + 1].glyph;
			if (lookup_char_code_0 == 0x223)
			{
				switch (cur_char_code)
				{
					case 0x1a1:
						dst_glyph = 0x267;
						need_trans = TRUE;
						break;
					case 0x1aa:
						dst_glyph = 0x268;
						need_trans = TRUE;
						break;
					case 0x1b4:
						dst_glyph = 0x269;
						need_trans = TRUE;
						break;
					default:
						break;
				}
				if (need_trans)
				{
					if (match_input(font_glyph_map, cur_select, lookup_index, &total_component_count, component_count, &is_mark_ligature))
					{
						ligate_input(font_glyph_map, cur_select, dst_glyph, total_component_count, component_count, is_mark_ligature);
					}
				}
			}
			break;
		case 33:
			if (cur_char_code == 0x20b)
			{
				khmer_replace_glyph_with_ligture(font_glyph_map, cur_select, 0x1f6, 0, FALSE);
			}
			else if (cur_char_code == 0x219 || cur_char_code == 0x21b || (cur_char_code >= 0x1ef && cur_char_code <= 0x1ff && ((cur_char_code - 0x1ef) % 2 == 0)) || (cur_char_code >= 0x21f && cur_char_code <= 0x243 && ((cur_char_code - 0x21f) % 2 == 0)))
			{
				khmer_replace_glyph_with_ligture(font_glyph_map, cur_select, cur_char_code + 1, 0, FALSE);
			}
			break;
		case 34:
			if (cur_char_code == 0x185)
			{
				khmer_replace_glyph_with_ligture(font_glyph_map, cur_select, 0x186, 0, FALSE);
			}
			break;
		case 35:
			if (cur_char_code == 0x11e)
			{
				khmer_replace_glyph_with_ligture(font_glyph_map, cur_select, 0x11f, 0, FALSE);
			}
			break;
		case 36:
			if (is_khmer_special_char(0, cur_char_code))
			{
				khmer_replace_glyph_with_ligture(font_glyph_map, cur_select, cur_char_code + 1, 0, FALSE);
			}
			break;
		case 37:
			if (is_khmer_special_char(0, cur_char_code) || (cur_char_code >= 0x204 && cur_char_code <= 0x205))
			{
				khmer_replace_glyph_with_ligture(font_glyph_map, cur_select, cur_char_code + 1, 0, FALSE);
			}
			break;
		case 38:
			if (cur_char_code == 0x1ab)
			{
				khmer_replace_glyph_with_ligture(font_glyph_map, cur_select, 0x1ac, 0, FALSE);
			}
			break;
		case 39:
			if (cur_char_code == 0x120)
			{
				khmer_replace_glyph_with_ligture(font_glyph_map, cur_select, 0x122, 0, FALSE);
			}
			break;
		case 40:
			if (cur_char_code == 0x201 || cur_char_code == 0x204 || cur_char_code == 0x207)
			{
				khmer_replace_glyph_with_ligture(font_glyph_map, cur_select, cur_char_code + 1, 0, FALSE);
			}
			break;
		case 41:
			if (cur_char_code == 0x201 || cur_char_code == 0x204 || cur_char_code == 0x207)
			{
				khmer_replace_glyph_with_ligture(font_glyph_map, cur_select, cur_char_code + 2, 0, FALSE);
			}
			break;
		case 42:
			if (cur_char_code == 0x20b)
			{
				khmer_replace_glyph_with_ligture(font_glyph_map, cur_select, 0x1f7, 0, FALSE);
			}
			else if ((cur_char_code >= 0x1ef && cur_char_code <= 0x1f0) || (cur_char_code >= 0x1f5 && cur_char_code <= 0x1f6)
					|| (cur_char_code >= 0x1f9 && cur_char_code <= 0x1fa) || (cur_char_code >= 0x1fd && cur_char_code <= 0x1fe))
			{
				khmer_replace_glyph_with_ligture(font_glyph_map, cur_select, cur_char_code + 2, 0, FALSE);
			}
			break;
		case 43:
			if ((cur_char_code >= 0x219 && cur_char_code <= 0x21a) || (cur_char_code >= 0x225 && cur_char_code <= 0x226) || (cur_char_code >= 0x22b && cur_char_code <= 0x22c)
					|| (cur_char_code >= 0x231 && cur_char_code <= 0x232) || (cur_char_code >= 0x237 && cur_char_code <= 0x238) || (cur_char_code >= 0x23d && cur_char_code <= 0x23e))
			{
				khmer_replace_glyph_with_ligture(font_glyph_map, cur_select, cur_char_code + 2, 0, FALSE);
			}
			break;
		case 44:
			if ((cur_char_code >= 0x22b && cur_char_code <= 0x22c) || (cur_char_code >= 0x231 && cur_char_code <= 0x232)
					|| (cur_char_code >= 0x237 && cur_char_code <= 0x238) || (cur_char_code >= 0x23d && cur_char_code <= 0x23e))
			{
				khmer_replace_glyph_with_ligture(font_glyph_map, cur_select, cur_char_code + 4, 0, FALSE);
			}
			break;
		case 45:
			if (is_khmer_special_char(11, cur_char_code) || cur_char_code == 0x20d || cur_char_code == 0x210)
			{
				khmer_replace_glyph_with_ligture(font_glyph_map, cur_select, cur_char_code + 1, 0, FALSE);
			}
			break;
		case 46:
			if (cur_char_code == 0x20d || cur_char_code == 0x210)
			{
				khmer_replace_glyph_with_ligture(font_glyph_map, cur_select, cur_char_code + 1, 0, FALSE);
			}
			break;
		case 47:
			if ((cur_select + 1) >= str_len)
			{
				break;
			}

			lookup_char_code_0 = font_glyph_map->font_fun_glyph[cur_select + 1].glyph;
			if (lookup_char_code_0 == 0x223 && cur_char_code == 0x15e)
			{
				dst_glyph = 0x266;
				if (match_input(font_glyph_map, cur_select, lookup_index, &total_component_count, component_count, &is_mark_ligature))
				{
					ligate_input(font_glyph_map, cur_select, dst_glyph, total_component_count, component_count, is_mark_ligature);
				}
			}
			break;
		case 49:
			if (cur_char_code == 0x21f)
			{
				khmer_replace_glyph_with_ligture(font_glyph_map, cur_select, 0x203, 0, FALSE);
			}
			break;
		case 50:
			printf("parse lookup index is 50 \n");
			if (cur_char_code == 0x223)
			{
				khmer_replace_glyph_with_ligture(font_glyph_map, cur_select, 0x266, 0, FALSE);
			}
			break;
		case 51:
			if (is_khmer_special_char(0, cur_char_code))
			{
				khmer_replace_glyph_with_ligture(font_glyph_map, cur_select, cur_char_code + 1, 0, FALSE);
			}
			break;
		case 52:
			printf("parse lookup index is 52 \n");
			break;
		default:
			break;
	}
}

static void khmer_text_transform_for_cur_lookup(int lookup_index, int cur_select, FONT_GLYPH_MAP *font_glyph_map)
{
	int str_len = font_glyph_map->font_glyph_num;
	BOOL need_trans = FALSE;
	U16 cur_char_code = font_glyph_map->font_fun_glyph[cur_select].glyph;
	U16 back_char_code_0 = 0;
	U16 back_char_code_1 = 0;
	U16 back_char_code_2 = 0;
	U16 lookup_char_code_0 = 0;
	U16 lookup_char_code_1 = 0;
	U16 lookup_char_code_2 = 0;
	U16 dst_glyph = 0;
	int char_info_size = sizeof(FONT_GLYPH);
	int total_component_count = 0;
	int component_count = 2;
	BOOL is_mark_ligature = FALSE;

	if ((font_glyph_map->font_fun_glyph[cur_select].font_glyph_get_metrics_func != get_khmer_char_metrics))
	{
		return;
	}

	if (!(check_glyph_mask(font_glyph_map, cur_select, lookup_index) && check_glyph_property(font_glyph_map, cur_select, lookup_index)))
	{
		return;
	}
	switch (lookup_index)
	{
		case 0:
			if ((cur_select + 1) >= str_len)
			{
				break;
			}

			lookup_char_code_0 = font_glyph_map->font_fun_glyph[cur_select + 1].glyph;
			if ((cur_char_code == 0x1ef) && (lookup_char_code_0 == 0x22b))
			{
				dst_glyph = 0x1f3;
				if (match_input(font_glyph_map, cur_select, lookup_index, &total_component_count, component_count, &is_mark_ligature))
				{
					ligate_input(font_glyph_map, cur_select, dst_glyph, total_component_count, component_count, is_mark_ligature);
				}
			}
			break;
		case 1:
			if ((cur_select + 1) >= str_len)
			{
				break;
			}

			lookup_char_code_0 = font_glyph_map->font_fun_glyph[cur_select + 1].glyph;
			if ((cur_char_code == 0x245) && (lookup_char_code_0 == 0x184))
			{
				dst_glyph = 0x185;
				if (match_input(font_glyph_map, cur_select, lookup_index, &total_component_count, component_count, &is_mark_ligature))
				{
					ligate_input(font_glyph_map, cur_select, dst_glyph, total_component_count, component_count, is_mark_ligature);
				}
			}
			break;
		case 2:
			if ((cur_select + 1) >= str_len)
			{
				break;
			}

			if (cur_char_code == 0x245)
			{
				lookup_char_code_0 = font_glyph_map->font_fun_glyph[cur_select + 1].glyph;
				switch (lookup_char_code_0)
				{
					case 0x11e:
						dst_glyph = 0x120;
						need_trans = TRUE;
						break;
					default:
						 if (is_khmer_special_char(6, lookup_char_code_0))
						 {
							 need_trans = TRUE;
							 dst_glyph = lookup_char_code_0 + 1;
						 }
						break;
				}

				if (need_trans)
				{
					if (match_input(font_glyph_map, cur_select, lookup_index, &total_component_count, component_count, &is_mark_ligature))
					{
						ligate_input(font_glyph_map, cur_select, dst_glyph, total_component_count, component_count, is_mark_ligature);
					}
				}
			}
			break;
		case 3:
			if ((cur_char_code == 0x21f))
			{
				if (get_back_track_0_transform_state(0, lookup_index, font_glyph_map, cur_select) && (get_lookup_ahead_0_transform_state(10, lookup_index, font_glyph_map, cur_select)
						|| get_lookup_ahead_0_transform_state(2, lookup_index, font_glyph_map, cur_select) || get_lookup_ahead_0_transform_state(3, lookup_index, font_glyph_map, cur_select)))
				{
					need_trans = TRUE;
				}
				else if (get_back_track_0_transform_state(0, lookup_index, font_glyph_map, cur_select) && get_lookup_ahead_0_transform_state(1, lookup_index, font_glyph_map, cur_select)
						&& get_lookup_ahead_1_transform_state(0, lookup_index, font_glyph_map, cur_select))
				{
					need_trans = TRUE;
				}

				if (need_trans)
				{
					khmer_text_transform_for_chain_context_lookup(30, cur_select, font_glyph_map);
				}
			}
			break;
		case 4:
			if ((cur_char_code == 0x223))
			{
				if (get_back_track_0_transform_state(1, lookup_index, font_glyph_map, cur_select) && (get_lookup_ahead_0_transform_state(10, lookup_index, font_glyph_map, cur_select)
						|| get_lookup_ahead_0_transform_state(2, lookup_index, font_glyph_map, cur_select)))
				{
					need_trans = TRUE;
				}
				else if (get_back_track_0_transform_state(2, lookup_index, font_glyph_map, cur_select) && get_lookup_ahead_0_transform_state(1, lookup_index, font_glyph_map, cur_select)
							&& get_lookup_ahead_1_transform_state(0, lookup_index, font_glyph_map, cur_select))
				{
					need_trans = TRUE;
				}

				if (need_trans)
				{
					khmer_text_transform_for_chain_context_lookup(31, cur_select, font_glyph_map);
				}
			}
			break;
		case 5:
			if ((cur_select + 2) >= str_len)
			{
				break;
			}

			if ((cur_char_code == 0x1a1 || cur_char_code == 0x1aa || cur_char_code == 0x1b4) || (cur_char_code == 0x223))
			{
				if (get_lookup_ahead_0_transform_state(4, lookup_index, font_glyph_map, cur_select) && (get_lookup_ahead_1_transform_state(1, lookup_index, font_glyph_map, cur_select)
						|| get_lookup_ahead_1_transform_state(2, lookup_index, font_glyph_map, cur_select) || get_lookup_ahead_1_transform_state(3, lookup_index, font_glyph_map, cur_select)
						|| get_lookup_ahead_1_transform_state(4, lookup_index, font_glyph_map, cur_select)))
				{
					khmer_text_transform_for_chain_context_lookup(32, cur_select, font_glyph_map);
				}
			}
			break;
		case 6:
			if (cur_char_code == 0x20a)
			{
				khmer_replace_glyph_with_ligture(font_glyph_map, cur_select, 0x20b, 0, FALSE);
			}
			break;
		case 7:
			if (cur_char_code == 0x20b || (cur_char_code == 0x219 || cur_char_code == 0x21b)
					|| (cur_char_code >= 0x1ef && cur_char_code <= 0x1ff && ((cur_char_code - 0x1ef) % 2 == 0))
					||  (cur_char_code >= 0x21f && cur_char_code <= 0x243 && ((cur_char_code - 0x21f) % 2 == 0)))
			{
				if (get_back_track_0_transform_state(3, lookup_index, font_glyph_map, cur_select))
				{
					need_trans = TRUE;
				}
				else if (get_back_track_0_transform_state(7, lookup_index, font_glyph_map, cur_select))
				{
					need_trans = TRUE;
				}
				else if (get_back_track_0_transform_state(4, lookup_index, font_glyph_map, cur_select) && get_back_track_1_transform_state(0, lookup_index, font_glyph_map, cur_select))
				{
					need_trans = TRUE;
				}
				else if (get_back_track_0_transform_state(5, lookup_index, font_glyph_map, cur_select) && get_back_track_1_transform_state(1, lookup_index, font_glyph_map, cur_select))
				{
					need_trans = TRUE;
				}
				else if (get_back_track_0_transform_state(6, lookup_index, font_glyph_map, cur_select) && get_back_track_1_transform_state(1, lookup_index, font_glyph_map, cur_select))
				{
					need_trans = TRUE;
				}

				if (need_trans)
				{
					khmer_text_transform_for_chain_context_lookup(33, cur_select, font_glyph_map);
				}
			}

			break;
		case 8:
			if (cur_char_code == 0x20c || cur_char_code == 0x20f || cur_char_code == 0x215 || cur_char_code == 0x217)
			{
				khmer_replace_glyph_with_ligture(font_glyph_map, cur_select, cur_char_code + 1, 0, FALSE);
			}
			else if (cur_char_code == 0x245 && (cur_select + 1 < str_len))
			{
				lookup_char_code_0 = font_glyph_map->font_fun_glyph[cur_select + 1].glyph;
				if (lookup_char_code_0 == 0xf8 || lookup_char_code_0 == 0x115 || lookup_char_code_0 == 0x137 || lookup_char_code_0 == 0x15e
						|| lookup_char_code_0 == 0x17b || lookup_char_code_0 == 0x198 || lookup_char_code_0 == 0x1a1)
				{
					if (match_input(font_glyph_map, cur_select, lookup_index, &total_component_count, component_count, &is_mark_ligature))
					{
						ligate_input(font_glyph_map, cur_select, lookup_char_code_0 + 1, total_component_count, component_count, is_mark_ligature);
					}
				}
			}
			break;
		case 9:
			if (cur_select + 2 >= str_len)
			{
				break;
			}

			if (cur_char_code == 0x185)
			{
				if (get_lookup_ahead_0_transform_state(5, lookup_index, font_glyph_map, cur_select) && (get_lookup_ahead_1_transform_state(5, lookup_index, font_glyph_map, cur_select)
						|| get_lookup_ahead_1_transform_state(6, lookup_index, font_glyph_map, cur_select)))
				{
					khmer_text_transform_for_chain_context_lookup(34, cur_select, font_glyph_map);
				}
			}
			break;
		case 10:
			if (cur_select + 1 >= str_len)
			{
				break;
			}

			if (cur_char_code == 0x11e)
			{
				if (get_lookup_ahead_0_transform_state(6, lookup_index, font_glyph_map, cur_select))
				{
					khmer_text_transform_for_chain_context_lookup(35, cur_select, font_glyph_map);
				}
			}
			break;
		case 11:
			if (cur_select - 3 < 0)
			{
				break;
			}

			if (is_khmer_special_char(0, cur_char_code) && (get_back_track_0_transform_state(8, lookup_index, font_glyph_map, cur_select) &&
					get_back_track_1_transform_state(2, lookup_index, font_glyph_map, cur_select) && get_back_track_2_transform_state(0, lookup_index, font_glyph_map, cur_select)))
			{
				khmer_text_transform_for_chain_context_lookup(36, cur_select, font_glyph_map);
			}
			break;
		case 12:
			if (cur_select + 1 >= str_len)
			{
				break;
			}

			lookup_char_code_0 = font_glyph_map->font_fun_glyph[cur_select + 1].glyph;
			if (is_khmer_special_char(0, cur_char_code) || (cur_char_code >= 0x204 && cur_char_code <= 0x245))
			{
				need_trans = TRUE;
			}

			if (need_trans && (get_lookup_ahead_0_transform_state(7, lookup_index, font_glyph_map, cur_select)))
			{
				khmer_text_transform_for_chain_context_lookup(37, cur_select, font_glyph_map);
			}
			break;
		case 13:
			if (cur_select - 1 < 0)
			{
				break;
			}

			back_char_code_0 = font_glyph_map->font_fun_glyph[cur_select - 1].glyph;
			if (cur_char_code == 0x1ab && back_char_code_0 == 0x1af)
			{
				khmer_text_transform_for_chain_context_lookup(38, cur_select, font_glyph_map);
			}
			break;
		case 14:
			if (cur_select - 1 < 0)
			{
				break;
			}

			back_char_code_0 = font_glyph_map->font_fun_glyph[cur_select - 1].glyph;
			if (cur_char_code == 0x120 && back_char_code_0 == 0x11f)
			{
				khmer_text_transform_for_chain_context_lookup(39, cur_select, font_glyph_map);
			}
			break;
		case 15:
			if (cur_char_code == 0x201 || cur_char_code == 0x204 || cur_char_code == 0x207)
			{
				if (get_back_track_0_transform_state(5, lookup_index, font_glyph_map, cur_select))
				{
				 need_trans = TRUE;
				}
				else if (get_back_track_0_transform_state(11, lookup_index, font_glyph_map, cur_select))
				{
				 need_trans = TRUE;
				}
				else if (get_back_track_0_transform_state(12, lookup_index, font_glyph_map, cur_select))
				{
				 need_trans = TRUE;
				}
				else if (get_back_track_0_transform_state(13, lookup_index, font_glyph_map, cur_select))
				{
				 need_trans = TRUE;
				}
				else if (get_back_track_0_transform_state(9, lookup_index, font_glyph_map, cur_select) && get_back_track_1_transform_state(3, lookup_index, font_glyph_map, cur_select))
				{
					 need_trans = TRUE;
				}
				else if (get_back_track_0_transform_state(10, lookup_index, font_glyph_map, cur_select) && (get_back_track_1_transform_state(2, lookup_index, font_glyph_map, cur_select)
					|| get_back_track_1_transform_state(4, lookup_index, font_glyph_map, cur_select) || get_back_track_1_transform_state(5, lookup_index, font_glyph_map, cur_select)))
				{
					need_trans = TRUE;
				}

				if (need_trans)
				{
					khmer_text_transform_for_chain_context_lookup(40, cur_select, font_glyph_map);
				}
			}
			break;
		case 16:
			if (cur_char_code == 0x201 || cur_char_code == 0x204 || cur_char_code == 0x207)
			{
				if (get_back_track_0_transform_state(14, lookup_index, font_glyph_map, cur_select))
				{
					need_trans = TRUE;
				}
				else if (get_back_track_0_transform_state(5, lookup_index, font_glyph_map, cur_select) && get_back_track_1_transform_state(6, lookup_index, font_glyph_map, cur_select)
						&& get_back_track_2_transform_state(1, lookup_index, font_glyph_map, cur_select))
				{
					need_trans = TRUE;
				}

				if (need_trans)
				{
					khmer_text_transform_for_chain_context_lookup(41, cur_select, font_glyph_map);
				}
			}
			break;
		case 17:
			if (is_khmer_special_char(8, cur_char_code))
			{
				if (get_back_track_0_transform_state(15, lookup_index, font_glyph_map, cur_select))
				{
					need_trans = TRUE;
				}
				else if (get_back_track_0_transform_state(17, lookup_index, font_glyph_map, cur_select))
				{
					need_trans = TRUE;
				}
				else if (get_back_track_0_transform_state(4, lookup_index, font_glyph_map, cur_select) && (get_back_track_1_transform_state(7, lookup_index, font_glyph_map, cur_select)
						|| get_back_track_1_transform_state(0, lookup_index, font_glyph_map, cur_select)))
				{
					need_trans = TRUE;
				}
				else if (get_back_track_0_transform_state(16, lookup_index, font_glyph_map, cur_select) && (get_back_track_1_transform_state(7, lookup_index, font_glyph_map, cur_select)
						|| get_back_track_1_transform_state(0, lookup_index, font_glyph_map, cur_select)))
				{
					need_trans = TRUE;
				}

				if (need_trans)
				{
					khmer_text_transform_for_chain_context_lookup(42, cur_select, font_glyph_map);
				}
			}
			break;
		case 18:
			if ((cur_char_code >= 0x219 && cur_char_code <= 0x21a) || (cur_char_code >= 0x225 && cur_char_code <= 0x226)
					|| (cur_char_code >= 0x22b && cur_char_code <= 0x22c) || (cur_char_code >= 0x231 && cur_char_code <= 0x232)
					|| (cur_char_code >= 0x237 && cur_char_code <= 0x238) || (cur_char_code >= 0x23d && cur_char_code <= 0x23e))
			{
				if (get_back_track_0_transform_state(18, lookup_index, font_glyph_map, cur_select))
				{
					need_trans = TRUE;
				}
				else if (get_back_track_0_transform_state(4, lookup_index, font_glyph_map, cur_select) && (get_back_track_1_transform_state(7, lookup_index, font_glyph_map, cur_select)
						|| get_back_track_1_transform_state(0, lookup_index, font_glyph_map, cur_select)))
				{
					need_trans = TRUE;
				}

				if (need_trans)
				{
					khmer_text_transform_for_chain_context_lookup(43, cur_select, font_glyph_map);
				}
			}
			break;
		case 19:
			if (cur_select - 1 < 0)
			{
				break;
			}

			if ((cur_char_code >= 0x22b && cur_char_code <= 0x22c) || (cur_char_code >= 0x231 && cur_char_code <= 0x232)
				|| (cur_char_code >= 0x237 && cur_char_code <= 0x238) || (cur_char_code >= 0x23d && cur_char_code <= 0x23e))
			{
				if (get_back_track_0_transform_state(19, lookup_index, font_glyph_map, cur_select))
				{
					khmer_text_transform_for_chain_context_lookup(44, cur_select, font_glyph_map);
				}
			}
			break;
		case 20:
			if (is_khmer_special_char(11, cur_char_code) || cur_char_code == 0x20d || cur_char_code == 0x210)
			{
				if (get_back_track_0_transform_state(20, lookup_index, font_glyph_map, cur_select))
				{
					need_trans = TRUE;
				}
				else if (get_back_track_0_transform_state(13, lookup_index, font_glyph_map, cur_select))
				{
					need_trans = TRUE;
				}
				else if (get_back_track_0_transform_state(9, lookup_index, font_glyph_map, cur_select) && get_back_track_1_transform_state(3, lookup_index, font_glyph_map, cur_select))
				{
					need_trans = TRUE;
				}

				if (need_trans)
				{
					khmer_text_transform_for_chain_context_lookup(45, cur_select, font_glyph_map);
				}
			}
			break;
		case 21:
			if (cur_select - 1 < 0)
			{
				break;
			}

			if ((cur_char_code == 0x20d || cur_char_code == 0x210) && get_back_track_0_transform_state(12, lookup_index, font_glyph_map, cur_select))
			{
				khmer_text_transform_for_chain_context_lookup(46, cur_select, font_glyph_map);
			}
			break;
		case 22:
			if (cur_select + 1 >= str_len)
			{
				break;
			}
			lookup_char_code_0 = font_glyph_map->font_fun_glyph[cur_select + 1].glyph;
			switch (cur_char_code)
			{
				case 0xe9:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0xec;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0xed;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0xee:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0x216:
							dst_glyph = 0xf1;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0xf2;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0xf3:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0xf6;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0xf7;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0xf8:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0xfb;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0xfe;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0xf9:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0x216:
							dst_glyph = 0xfc;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0xff;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0xfa:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0x216:
							dst_glyph = 0xfd;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x100;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x101:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0x104;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x105;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x106:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0x109;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x10a;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x10b:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0x10e;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x10f;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x110:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0x113;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x114;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x115:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0x118;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x11b;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x116:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0x216:
							dst_glyph = 0x119;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x11c;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x117:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0x216:
							dst_glyph = 0x11a;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x11d;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x11e:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0x124;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x126;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x11f:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0x216:
							dst_glyph = 0x125;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x127;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x128:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0x12b;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x12c;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x12d:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0x130;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x131;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x132:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0x135;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x136;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x137:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0x13a;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x13d;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x138:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0x216:
							dst_glyph = 0x13b;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x13e;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x139:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0x216:
							dst_glyph = 0x13c;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x13f;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x140:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0x143;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x144;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x145:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0x148;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x149;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x14a:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0x14d;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x14e;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x14f:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0x152;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x153;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x154:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0x157;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x158;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x159:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0x15c;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x15d;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x15e:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0x161;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x164;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x15f:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0x216:
							dst_glyph = 0x162;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x165;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x160:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0x216:
							dst_glyph = 0x163;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x166;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x167:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0x16a;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x16b;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x16c:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0x16f;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x170;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x171:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0x174;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x175;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x176:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0x179;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x17a;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x17b:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0x17e;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x181;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x17c:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0x216:
							dst_glyph = 0x17f;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x182;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x17d:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0x216:
							dst_glyph = 0x180;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x183;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x184:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0x187;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x188;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x189:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0x18c;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x18d;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x18e:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0x191;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x192;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x193:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0x196;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x197;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x198:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0x216:
							dst_glyph = 0x19b;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x19e;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x199:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0x216:
							dst_glyph = 0x19c;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x19f;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x19a:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0x216:
							dst_glyph = 0x19d;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x1a0;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x1a1:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0xe8:
						case 0x216:
							dst_glyph = 0x1a4;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x1a7;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x1a2:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0x216:
							dst_glyph = 0x1a5;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x1a8;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x1a3:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0x216:
							dst_glyph = 0x1a6;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x1a9;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x1aa:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0xe8:
						case 0x216:
							dst_glyph = 0x1ad;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x1ae;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x1af:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0xe8:
						case 0x216:
							dst_glyph = 0x1b2;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x1b3;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				case 0x1b4:
				case 0x1b9:
					switch (lookup_char_code_0)
					{
						case 0x1ee:
						case 0xe7:
						case 0xe8:
						case 0x216:
							dst_glyph = 0x1b7;
							need_trans = TRUE;
							break;
						case 0x218:
							dst_glyph = 0x1b8;
							need_trans = TRUE;
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}

			if (lookup_char_code_0 == 0xe7 || lookup_char_code_0 == 0xe8)
			{
				need_trans = FALSE;
				if (cur_select + 2 < str_len)
				{
					if (font_glyph_map->font_fun_glyph[cur_select + 2].glyph == 0x1ee)
					{
						need_trans = TRUE;
						component_count = 3;
					}
				}
			}
			if (need_trans)
			{
				if (match_input(font_glyph_map, cur_select, lookup_index, &total_component_count, component_count, &is_mark_ligature))
				{
					ligate_input(font_glyph_map, cur_select, dst_glyph, total_component_count, component_count, is_mark_ligature);
				}
			}
			break;
		case 23:
			if ((cur_select + 1) >= str_len)
			{
				break;
			}

			if (cur_char_code == 0x15e || cur_char_code == 0x223)
			{
				if (get_lookup_ahead_0_transform_state(9, lookup_index, font_glyph_map, cur_select))
				{
					khmer_text_transform_for_chain_context_lookup(47, cur_select, font_glyph_map);
				}
			}
			break;
		case 24:
			if ((cur_select + 1) >= str_len)
			{
				break;
			}

			if (cur_char_code == 0x1a1 || cur_char_code == 0x1aa || cur_char_code == 0x1b4 || cur_char_code == 0x223)
			{
				if (get_lookup_ahead_0_transform_state(10, lookup_index, font_glyph_map, cur_select))
				{
					khmer_text_transform_for_chain_context_lookup(48, cur_select, font_glyph_map);
				}
			}
			break;
		case 25:
			if (cur_char_code == 0x21f)
			{

				if (get_back_track_0_transform_state(0, lookup_index, font_glyph_map, cur_select) && get_lookup_ahead_0_transform_state(0, lookup_index, font_glyph_map, cur_select)
						&& get_lookup_ahead_1_transform_state(7, lookup_index, font_glyph_map, cur_select))
				{
					need_trans = TRUE;
				}

				else if (get_back_track_0_transform_state(5, lookup_index, font_glyph_map, cur_select) && get_back_track_1_transform_state(8, lookup_index, font_glyph_map, cur_select)
						&& get_lookup_ahead_0_transform_state(10, lookup_index, font_glyph_map, cur_select))
				{
					need_trans = TRUE;
				}

				if (need_trans)
				{
					khmer_text_transform_for_chain_context_lookup(49, cur_select, font_glyph_map);
				}
			}
			break;
		case 26:
			printf("parse lookup index is 26 \n");
			if ((cur_select + 1) >= str_len)
			{
				break;
			}
			lookup_char_code_0 = font_glyph_map->font_fun_glyph[cur_select + 1].glyph;
			if (cur_char_code == 0x223 && lookup_char_code_0 == 0x1ef)
			{
				khmer_text_transform_for_chain_context_lookup(50, cur_select, font_glyph_map);
			}
			break;
		case 27:
			if (((cur_select + 1) >= str_len) || ((cur_select - 1) < 0))
			{
				break;
			}

			if (is_khmer_special_char(0, cur_char_code) && get_back_track_0_transform_state(9, lookup_index, font_glyph_map, cur_select)
					&& get_lookup_ahead_0_transform_state(11, lookup_index, font_glyph_map, cur_select))
			{
				khmer_text_transform_for_chain_context_lookup(51, cur_select, font_glyph_map);
			}

			break;
		case 28:
			printf("parse lookup index is 28 \n");
			if ((cur_select + 2) >= str_len)
			{
				break;
			}

			if (cur_char_code == 0x223)
			{
				if (get_lookup_ahead_0_transform_state(8, lookup_index, font_glyph_map, cur_select) && get_lookup_ahead_1_transform_state(7, lookup_index, font_glyph_map, cur_select))
				{
					khmer_text_transform_for_chain_context_lookup(52, cur_select, font_glyph_map);
				}
			}
			break;
		case 29:
			if ((cur_select + 1) >= str_len)
			{
				break;
			}

			lookup_char_code_0 = font_glyph_map->font_fun_glyph[cur_select + 1].glyph;
			if ((cur_char_code == 0x11e) && (lookup_char_code_0 == 0x204))
			{
				dst_glyph = 0x2a2;
				if (match_input(font_glyph_map, cur_select, lookup_index, &total_component_count, component_count, &is_mark_ligature))
				{
					ligate_input(font_glyph_map, cur_select, dst_glyph, total_component_count, component_count, is_mark_ligature);
				}
			}
			break;
		default:
			break;
	}
}

BOOL khmer_transform_text(FONT_GLYPH_MAP *font_glyph_map)
{
	BOOL ret = FALSE;
	U32 glyph_index;
	int count = 0;
	char parse_lookup_seq_temp[14] = {0, 29, 1, 2, 3, 4, 5, 25, 27, 6, 7, 8, 0, 9};
	char parse_lookup_seq[29] = {0};
	int seq_index = 0;
	int text_len = font_glyph_map->font_glyph_num;

	for (count = 0; count < sizeof(parse_lookup_seq); count++)
	{
		if (count < sizeof(parse_lookup_seq_temp))
		{
			parse_lookup_seq[count] = parse_lookup_seq_temp[count];
		}
		else
		{
			parse_lookup_seq[count] = 10 + (count - sizeof(parse_lookup_seq_temp));
			if (parse_lookup_seq[count] > 24)
			{
				break;
			}
		}
	}
	set_khmer_text_glyph_props(font_glyph_map);
	setup_syllables(font_glyph_map);
	reset_glyph_mask(font_glyph_map);

	for (seq_index = 0; seq_index < sizeof(parse_lookup_seq); seq_index++)
	{
		if (is_pref_lookup(parse_lookup_seq[seq_index]))
		{
			initial_reordering(font_glyph_map);
		}

		if (is_blwm_lookup(parse_lookup_seq[seq_index], seq_index))
		{
			final_reordering(font_glyph_map);
		}

		for (count = 0; count < text_len; count++)
		{
			khmer_text_transform_for_cur_lookup(parse_lookup_seq[seq_index], count, font_glyph_map);
		}
	}
	serial_lig_count = 0;

	return ret;
}

#if 0
static BOOL is_khmer_unicode_general_category(U16 char_code)
{
	BOOL ret = FALSE;

	if ((char_code >= 0x1780 && char_code <= 0x17b3) || (char_code >= 0x17d4 && char_code <= 0x17dc)
			|| (char_code >= 0x17e0 && char_code <= 0x17e9) || (char_code >= 0x17f0 && char_code <= 0x17f9))
	{
		ret = TRUE;
	}

	return ret;
}

static void khmer_merge_cluster(U16 *cluster, int str_len, int start, int end, int cur_select)
{
	U16 cluster_temp = cluster[start];
	int count = 0;

	if (end - start < 2)
		return;

    for (count = start + 1; count < end; count++)
    {
    	cluster_temp = MIN(cluster_temp, cluster[count]);
    }

    /* Extend end */
    if (end < str_len && end > 0)
    {
		while (cluster[end - 1] == cluster[end])
		{
		  end++;
		}
    }
    /* Extend start */
	if (cur_select < start && start > 0)
	{
		while (cluster[start - 1] == cluster[start])
		{
			start--;
		}
	}

	for (count = start; count < end; count++)
	{
		cluster[count] = cluster_temp;
	}
}

void khmer_form_cluster(U16 *ucs2_string, U16 *cluster)
{
	int str_len = wcslen(ucs2_string);
	int count = 0;
	int start = 0;
	U16 temp_cluster = 0;

	for (count = 0; count < str_len; count++)
	{
		cluster[count] = temp_cluster;
		temp_cluster += ucs2_char_to_utf8_buf(ucs2_string[count], NULL);
	}

	for (count = 1; count < str_len; count++)
	{
		if (is_khmer_unicode_general_category(ucs2_string[count]))
		{
			khmer_merge_cluster(cluster, str_len, start, count, 0);
			start = count;
		}
	}

	khmer_merge_cluster(cluster, str_len, start, str_len, 0);
}
#endif

void khmer_text_decompos(U16 *ucs2_string)
{
	int index = 0;
	int info_len = wcslen(ucs2_string);
	BOOL need_decompose = FALSE;
	U16 temp_unicode[2] = {0};

	for (index = 0; index < info_len; index++)
	{
		switch (ucs2_string[index])
		{
			case 0x17BE:
				temp_unicode[0] = 0x17C1;
				temp_unicode[1] = 0x17BE;
				need_decompose = TRUE;
				break;
			case 0x17BF:
				temp_unicode[0] = 0x17C1;
				temp_unicode[1]= 0x17BF;
				need_decompose = TRUE;
				break;
			case 0x17C0:
				temp_unicode[0] = 0x17C1;
				temp_unicode[1]= 0x17C0;
				need_decompose = TRUE;
				break;
			case 0x17C4:
				temp_unicode[0] = 0x17C1;
				temp_unicode[1]= 0x17C4;
				need_decompose = TRUE;
				break;
			case 0x17C5:
				temp_unicode[0] = 0x17C1;
				temp_unicode[1]= 0x17C5;
				need_decompose = TRUE;
				break;
		}

		if (need_decompose)
		{
			memmove(&ucs2_string[index + 2], &ucs2_string[index + 1], (info_len - index - 1) * sizeof(U16));
			ucs2_string[index] = temp_unicode[0];
			ucs2_string[index + 1] = temp_unicode[1];
			info_len += 1;
			index++;
			ucs2_string[info_len] = 0;
			need_decompose = FALSE;
		}
	}

}

BOOL is_special_base_khmer_char(FONT_GLYPH font_glyph)
{
	BOOL ret = FALSE;
	U16 glyph_char = font_glyph.glyph;

	if (!font_glyph.font_glyph_get_metrics_func)
	{
		return ret;
	}

	if (glyph_char == 0x1af || glyph_char == 0x1b2 || glyph_char == 0x1b3)
	{
		ret = TRUE;
	}

	return ret;
}
#endif
