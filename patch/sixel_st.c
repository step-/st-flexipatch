sixel_state_t sixel_st;

void
dcshandle(void)
{
	int bgcolor;
	unsigned char r, g, b, a = 255;

	switch (csiescseq.mode[0]) {
	default:
		fprintf(stderr, "erresc: unknown csi ");
		csidump();
		/* die(""); */
		break;
	case 'q': /* DECSIXEL */
		if (IS_TRUECOL(term.c.attr.bg)) {
			r = term.c.attr.bg >> 16 & 255;
			g = term.c.attr.bg >> 8 & 255;
			b = term.c.attr.bg >> 0 & 255;
		} else {
			xgetcolor(term.c.attr.bg, &r, &g, &b);
			#if ALPHA_PATCH
			if (term.c.attr.bg == defaultbg)
				a = dc.col[defaultbg].pixel >> 24 & 255;
			#endif // ALPHA_PATCH
		}
		bgcolor = a << 24 | b << 16 | g << 8 | r;
		if (sixel_parser_init(&sixel_st, 255 << 24, bgcolor, 1, win.cw, win.ch) != 0)
			perror("sixel_parser_init() failed");
		term.mode |= MODE_SIXEL;
		break;
	}
}

void
scroll_images(int n) {
	ImageList *im;
	int top = IS_SET(MODE_ALTSCREEN) ? 0 : -HISTSIZE;

	for (im = term.images; im; im = im->next) {
		im->y += n;

		/* check if the current sixel has exceeded the maximum
		 * draw distance, and should therefore be deleted */
		if (im->y + (im->height + win.ch-1)/win.ch - 1 < top) {
			fprintf(stderr, "im@0x%08x exceeded maximum distance\n");
			im->should_delete = 1;
		}
	}
}
