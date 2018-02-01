SciArray<SciVector2> pen_pixs; // Interpenetrating pixels
bool pixs_inside = false; // Pixels are interpenetrated inside each-other

// Check every pixel inside collision box find colliding pixel
for(int i = 0; i < h; ++i)
{
	for(int j = 0; j < w; ++j)
	{
		// Calculate pixel world coordinates
		const SciVector2 pix_w = cmin + SciVector2((float)j, (float)i);
		// Map world pixel coordinates to local sprite space
		const SciVector2 pix_a = pix_w - amin;
		const SciVector2 pix_b = pix_w - bmin;
		const int ax = sciFloorI(pix_a.x);
		const int ay = sciFloorI(pix_a.y);
		const int bx = sciFloorI(pix_b.x);
		const int by = sciFloorI(pix_b.y);
		// Precompute pixels around current pixel
		const SciVector2 p_bot = pix_w - SciVector2(0.0f, 1.0f);
		const SciVector2 p_top = pix_w + SciVector2(0.0f, 1.0f);
		const SciVector2 p_left = pix_w - SciVector2(1.0f, 0.0f);
		const SciVector2 p_right = pix_w + SciVector2(1.0f, 0.0f);
		const SciVector2 p_bl = pix_w - SciVector2(1.0f, 1.0f);
		const SciVector2 p_br = pix_w + SciVector2(1.0f, -1.0f);
		const SciVector2 p_tl = pix_w + SciVector2(-1.0f, 1.0f);
		const SciVector2 p_tr = pix_w + SciVector2(1.0f, 1.0f);

		// If pixel in sprite a is solid, check pixels of b around it
		if(aspr(ax, ay).a)
		{
			// Pixels are inside one-another penetration depth at least 1
			if(bspr(bx, by).a)
			{
				pen_pixs.Push(pix_w);
				pixs_inside = true;
				continue;
			}
			// Pixels are on top/bot/left/right of each other, pen-depth 0
			const int bh = bspr.Height();
			const int bw = bspr.Width();
			if(by > 0 && bspr(bx, by-1).a)
			{
				pen_pixs.Push(pix_w);
				pen_pixs.Push(p_bot);
			}
			if(by < bh && bspr(bx, by+1).a)
			{
				pen_pixs.Push(pix_w);
				pen_pixs.Push(p_top);
			}
			if(bx > 0 && bspr(bx-1, by).a)
			{
				pen_pixs.Push(pix_w);
				pen_pixs.Push(p_left);
			}
			if(bx < bw && bspr(bx+1, by).a)
			{
				pen_pixs.Push(pix_w);
				pen_pixs.Push(p_right);
			}
			// Pixels are touching corners of one-another, pen-depth 0
			if(by > 0 && bx > 0 && bspr(bx-1, by-1).a)
			{
				pen_pixs.Push(pix_w);
				pen_pixs.Push(p_bl);
			}
			if(by > 0 && bx < bw && bspr(bx+1, by-1).a)
			{
				pen_pixs.Push(pix_w);
				pen_pixs.Push(p_br);
			}
			if(by < bh && bx > 0 && bspr(bx-1, by+1).a)
			{
				pen_pixs.Push(pix_w);
				pen_pixs.Push(p_tl);
			}
			if(by < bh && bx < bw && bspr(bx+1, by+1).a)
			{
				pen_pixs.Push(pix_w);
				pen_pixs.Push(p_tr);
			}
		}

		//// Check if pixels at this coordinates are solid
		//const bool a_solid = (bool)aspr(sciFloorI(pix_a.x), sciFloorI(pix_a.y)).a;
		//const bool b_solid = (bool)bspr(sciFloorI(pix_b.x), sciFloorI(pix_b.y)).a;
		//// If both a and b pixels at thie position are solid, they are colliding
		//if(a_solid && b_solid)
		//{
		//	pen_pixs.Push(pix_w);
		//}
	}
}