/*
 * arabic_transform.c
 *
 *  Created on: 2016-2-1
 *      Author: Brant
 */

#include <gdi/gui_interface.h>

#define NIL 0x0000

typedef struct
{
	unsigned short code;
	unsigned short isolated;
	unsigned short initial;
	unsigned short medial;
	unsigned short final;
} ArabicCharRep;

typedef struct
{
	unsigned short code[2];
	unsigned short isolated;
	unsigned short initial;
	unsigned short medial;
	unsigned short final;
} ArabicCombineCharRep;

static ArabicCharRep arabic_tramsform_chars_map[] =
{
	{0x0621, 0xFE80, NIL   , NIL   , NIL   }, /* ARABIC LETTER HAMZA */
	{0x0622, 0xFE81, NIL   , NIL   , 0xFE82}, /* ARABIC LETTER ALEF WITH MADDA ABOVE */
	{0x0623, 0xFE83, NIL   , NIL   , 0xFE84}, /* ARABIC LETTER ALEF WITH HAMZA ABOVE */
	{0x0624, 0xFE85, NIL   , NIL   , 0xFE86}, /* ARABIC LETTER WAW WITH HAMZA ABOVE */
	{0x0625, 0xFE87, NIL   , NIL   , 0xFE88}, /* ARABIC LETTER ALEF WITH HAMZA BELOW */
	{0x0626, 0xFE89, 0xFE8B, 0xFE8C, 0xFE8A}, /* ARABIC LETTER YEH WITH HAMZA ABOVE */
	{0x0627, 0xFE8D, NIL   , NIL   , 0xFE8E}, /* ARABIC LETTER ALEF */
	{0x0628, 0xFE8F, 0xFE91, 0xFE92, 0xFE90}, /* ARABIC LETTER BEH */
	{0x0629, 0xFE93, NIL   , NIL   , 0xFE94}, /* ARABIC LETTER TEH MARBUTA */
	{0x062A, 0xFE95, 0xFE97, 0xFE98, 0xFE96}, /* ARABIC LETTER TEH */
	{0x062B, 0xFE99, 0xFE9B, 0xFE9C, 0xFE9A}, /* ARABIC LETTER THEH */
	{0x062C, 0xFE9D, 0xFE9F, 0xFEA0, 0xFE9E}, /* ARABIC LETTER JEEM */
	{0x062D, 0xFEA1, 0xFEA3, 0xFEA4, 0xFEA2}, /* ARABIC LETTER HAH */
	{0x062E, 0xFEA5, 0xFEA7, 0xFEA8, 0xFEA6}, /* ARABIC LETTER KHAH */
	{0x062F, 0xFEA9, NIL   , NIL   , 0xFEAA}, /* ARABIC LETTER DAL */
	{0x0630, 0xFEAB, NIL   , NIL   , 0xFEAC}, /* ARABIC LETTER THAL */
	{0x0631, 0xFEAD, NIL   , NIL   , 0xFEAE}, /* ARABIC LETTER REH */
	{0x0632, 0xFEAF, NIL   , NIL   , 0xFEB0}, /* ARABIC LETTER ZAIN */
	{0x0633, 0xFEB1, 0xFEB3, 0xFEB4, 0xFEB2}, /* ARABIC LETTER SEEN */
	{0x0634, 0xFEB5, 0xFEB7, 0xFEB8, 0xFEB6}, /* ARABIC LETTER SHEEN */
	{0x0635, 0xFEB9, 0xFEBB, 0xFEBC, 0xFEBA}, /* ARABIC LETTER SAD */
	{0x0636, 0xFEBD, 0xFEBF, 0xFEC0, 0xFEBE}, /* ARABIC LETTER DAD */
	{0x0637, 0xFEC1, 0xFEC3, 0xFEC4, 0xFEC2}, /* ARABIC LETTER TAH */
	{0x0638, 0xFEC5, 0xFEC7, 0xFEC8, 0xFEC6}, /* ARABIC LETTER ZAH */
	{0x0639, 0xFEC9, 0xFECB, 0xFECC, 0xFECA}, /* ARABIC LETTER AIN */
	{0x063A, 0xFECD, 0xFECF, 0xFED0, 0xFECE}, /* ARABIC LETTER GHAIN */
	{0x0640, 0x0640, NIL   , NIL   , NIL   }, /* ARABIC LETTER TATWEEL */
	{0x0641, 0xFED1, 0xFED3, 0xFED4, 0xFED2}, /* ARABIC LETTER FEH */
	{0x0642, 0xFED5, 0xFED7, 0xFED8, 0xFED6}, /* ARABIC LETTER QAF */
	{0x0643, 0xFED9, 0xFEDB, 0xFEDC, 0xFEDA}, /* ARABIC LETTER KAF */
	{0x0644, 0xFEDD, 0xFEDF, 0xFEE0, 0xFEDE}, /* ARABIC LETTER LAM */
	{0x0645, 0xFEE1, 0xFEE3, 0xFEE4, 0xFEE2}, /* ARABIC LETTER MEEM */
	{0x0646, 0xFEE5, 0xFEE7, 0xFEE8, 0xFEE6}, /* ARABIC LETTER NOON */
	{0x0647, 0xFEE9, 0xFEEB, 0xFEEC, 0xFEEA}, /* ARABIC LETTER HEH */
	{0x0648, 0xFEED, NIL   , NIL   , 0xFEEE}, /* ARABIC LETTER WAW */
	{0x0649, 0xFEEF, 0xFEF3, 0xFEF4, 0xFEF0}, /* ARABIC LETTER ALEF MAKSURA */
	{0x064A, 0xFEF1, 0xFEF3, 0xFEF4, 0xFEF2}, /* ARABIC LETTER YEH */
	{0x0679, 0xFB66, 0xFB68, 0xFB69, 0xFB67}, /* ARABIC LETTER TTEH */
	{0x067E, 0xFB56, 0xFB58, 0xFB59, 0xFB57}, /* ARABIC LETTER PEH */
	{0x0686, 0xFB7A, 0xFB7C, 0xFB7D, 0xFB7B}, /* ARABIC LETTER TCHEH */
	{0x0688, 0xFB88, NIL   , NIL   , 0xFB89}, /* ARABIC LETTER DDAL */
	{0x0691, 0xFB8C, NIL   , NIL   , 0xFB8D}, /* ARABIC LETTER RREH */
	{0x0698, 0xFB8A, NIL   , NIL   , 0xFB8B}, /* ARABIC LETTER JEH */
	{0x06A9, 0xFB8E, 0xFB90, 0xFB91, 0xFB8F}, /* ARABIC LETTER KEHEH */
	{0x06AF, 0xFB92, 0xFB94, 0xFB95, 0xFB93}, /* ARABIC LETTER GAF */
	{0x06BA, 0xFB9E, 0xFEE7, 0xFEE8, 0xFB9F}, /* ARABIC LETTER NOON GHUNNA */
	{0x06BE, 0xFBAA, 0xFBAC, 0xFBAD, 0xFBAB}, /* ARABIC LETTER HEH DOACHASHMEE */
	{0x06C1, 0xFBA6, 0xFBA8, 0xFBA9, 0xFBA7}, /* ARABIC LETTER HEH GOAL */
	{0x06CC, 0xFEEF, 0xFEF3, 0xFEF4, 0xFEF0}, /* ARABIC LETTER FARSI YEH */
};

static ArabicCombineCharRep arabic_combine_chars_map[] =
{
	{{0x0644, 0x0622}, 0xFEF5, NIL, NIL, 0xFEF6}, /* LAM_ALEF_MADDA */
	{{0x0644, 0x0623}, 0xFEF7, NIL, NIL, 0xFEF8}, /* LAM_ALEF_HAMZA_ABOVE */
	{{0x0644, 0x0625}, 0xFEF9, NIL, NIL, 0xFEFA}, /* LAM_ALEF_HAMZA_BELOW */
	{{0x0644, 0x0627}, 0xFEFB, NIL, NIL, 0xFEFC}  /* LAM_ALEF */
};

static U16 arabic_trans_chars[] =
{
	0x0610, /* ARABIC SIGN SALLALLAHOU ALAYHE WASSALLAM */
	0x0612, /* ARABIC SIGN ALAYHE ASSALLAM */
	0x0613, /* ARABIC SIGN RADI ALLAHOU ANHU */
	0x0614, /* ARABIC SIGN TAKHALLUS */
	0x0615, /* ARABIC SMALL HIGH TAH */
	0x064B, /* ARABIC FATHATAN */
	0x064C, /* ARABIC DAMMATAN */
	0x064D, /* ARABIC KASRATAN */
	0x064E, /* ARABIC FATHA */
	0x064F, /* ARABIC DAMMA */
	0x0650, /* ARABIC KASRA */
	0x0651, /* ARABIC SHADDA */
	0x0652, /* ARABIC SUKUN */
	0x0653, /* ARABIC MADDAH ABOVE */
	0x0654, /* ARABIC HAMZA ABOVE */
	0x0655, /* ARABIC HAMZA BELOW */
	0x0656, /* ARABIC SUBSCRIPT ALEF */
	0x0657, /* ARABIC INVERTED DAMMA */
	0x0658, /* ARABIC MARK NOON GHUNNA */
	0x0670, /* ARABIC LETTER SUPERSCRIPT ALEF */
	0x06D6, /* ARABIC SMALL HIGH LIGATURE SAD WITH LAM WITH ALEF MAKSURA */
	0x06D7, /* ARABIC SMALL HIGH LIGATURE QAF WITH LAM WITH ALEF MAKSURA */
	0x06D8, /* ARABIC SMALL HIGH MEEM INITIAL FORM */
	0x06D9, /* ARABIC SMALL HIGH LAM ALEF */
	0x06DA, /* ARABIC SMALL HIGH JEEM */
	0x06DB, /* ARABIC SMALL HIGH THREE DOTS */
	0x06DC, /* ARABIC SMALL HIGH SEEN */
	0x06DF, /* ARABIC SMALL HIGH ROUNDED ZERO */
	0x06E0, /* ARABIC SMALL HIGH UPRIGHT RECTANGULAR ZERO */
	0x06E1, /* ARABIC SMALL HIGH DOTLESS HEAD OF KHAH */
	0x06E2, /* ARABIC SMALL HIGH MEEM ISOLATED FORM */
	0x06E3, /* ARABIC SMALL LOW SEEN */
	0x06E4, /* ARABIC SMALL HIGH MADDA */
	0x06E7, /* ARABIC SMALL HIGH YEH */
	0x06E8, /* ARABIC SMALL HIGH NOON */
	0x06EA, /* ARABIC EMPTY CENTRE LOW STOP */
	0x06EB, /* ARABIC EMPTY CENTRE HIGH STOP */
	0x06EC, /* ARABIC ROUNDED HIGH STOP WITH FILLED CENTRE */
	0x06ED  /* ARABIC SMALL LOW MEEM */
};


static int ucs2_string_len(const U16 *ucs2_string)
{
	int len = 0;

	if (!ucs2_string) return 0;

	while (*(ucs2_string++)) len++;

	return len;
}

static BOOL arabic_tramsform_map_contains(U16 char_code)
{
	int index;

	for (index = 0; index < sizeof(arabic_tramsform_chars_map) / sizeof(arabic_tramsform_chars_map[0]); index++)
	{
		if (arabic_tramsform_chars_map[index].code == char_code)
		{
			return TRUE;
		}
	}
	return FALSE;
}

static ArabicCharRep get_arabic_transform_char_rep(U16 char_code)
{
	ArabicCharRep char_rep = {NIL, NIL, NIL, NIL, NIL};
	int index;

	for (index = 0; index < sizeof(arabic_tramsform_chars_map) / sizeof(arabic_tramsform_chars_map[0]); index++)
	{
		if (arabic_tramsform_chars_map[index].code == char_code)
		{
			char_rep = arabic_tramsform_chars_map[index];
			break;
		}
	}
	return char_rep;
}

static ArabicCombineCharRep get_arabic_combine_char_rep(U16 char_code1, U16 char_code2)
{
	ArabicCombineCharRep combine_char_rep = {{NIL, NIL}, NIL, NIL, NIL, NIL};
	int index = 0;

	for (index = 0; index < sizeof(arabic_combine_chars_map) / sizeof(arabic_combine_chars_map[0]); index++)
	{
		if (arabic_combine_chars_map[index].code[0] == char_code1 && arabic_combine_chars_map[index].code[1] == char_code2)
		{
			combine_char_rep = arabic_combine_chars_map[index];
			break;
		}
	}
	return combine_char_rep;
}

BOOL is_arabic_transparent_char(U16 char_code)
{
	int index = 0;

	for (index = 0; index < sizeof(arabic_trans_chars) / sizeof(arabic_trans_chars[0]); index++)
	{
		if (arabic_trans_chars[index] == char_code)
		{
			return TRUE;
		}
	}
	return FALSE;
}

int arabic_text_transform(U16 *dst, const U16 *src)
{
	int src_length;
	int src_index;
	int dst_index;
	U16 current_char;
	ArabicCharRep char_rep;
	ArabicCombineCharRep combine_rep;

	src_length = ucs2_string_len(src);
	dst_index = 0;
	for (src_index = 0; src_index < src_length; ++src_index)
	{
		current_char = src[src_index];
		if (arabic_tramsform_map_contains(current_char))
		{
			U16 prev = NIL;
			U16 next = NIL;
			int prev_id = src_index - 1;
			int next_id = src_index + 1;

			/*
			 * Transparent characters have no effect in the shaping process.
			 * So, ignore all the transparent characters that are BEFORE the
			 * current character.
			 */
			for (; prev_id >= 0; prev_id--)
			{
				if (!is_arabic_transparent_char(src[prev_id]))
				{
					break;
				}
			}
			if ((prev_id < 0) || !arabic_tramsform_map_contains(prev = src[prev_id])
				|| (!((char_rep = get_arabic_transform_char_rep(prev)).initial != NIL) && !(char_rep.medial != NIL)))
			{
				prev = NIL;
			}

			/*
			 * Transparent characters have no effect in the shaping process.
			 * So, ignore all the transparent characters that are AFTER the
			 * current character.
			 */
			for (; next_id < src_length; next_id++)
			{
				if (!is_arabic_transparent_char(src[next_id]))
				{
					break;
				}
			}
			if ((next_id >= src_length) || !arabic_tramsform_map_contains(next = src[next_id])
				|| (!((char_rep = get_arabic_transform_char_rep(next)).medial != NIL) && !(char_rep.final != NIL) && (next != 0x0640)))
			{
				next = NIL;
			}

			/* Combinations */
			if (current_char == 0x0644 && (next == 0x0622 || next == 0x0623 || next == 0x0625 || next == 0x0627))
			{
				combine_rep = get_arabic_combine_char_rep(current_char, next);
				if (prev != NIL)
				{
					dst[dst_index++] = combine_rep.final;
				}
				else
				{
					dst[dst_index++] = combine_rep.isolated;
				}
				src_index++;
				continue;
			}

			char_rep = get_arabic_transform_char_rep(current_char);
			if (prev != NIL && next != NIL && char_rep.medial != NIL)	/* Medial */
			{
				dst[dst_index++] = char_rep.medial;
				continue;
			}
			else if (prev != NIL && char_rep.final != NIL)	/* Final */
			{
				dst[dst_index++] = char_rep.final;
				continue;
			}
			else if (next != NIL && char_rep.initial != NIL)	/* Initial */
			{
				dst[dst_index++] = char_rep.initial;
				continue;
			}
			/* Isolated */
			dst[dst_index++] = char_rep.isolated;
		}
		else
		{
			dst[dst_index++] = current_char;
		}
	}
	dst[dst_index] = NIL;

	return 0;
}

