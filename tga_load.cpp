/* TARGA file must be 32 bpp, correct resolution, with 0|0 left-bottom origin,
no Color Map, no Image Identification */
template <>
bool SciResourceNan<SciPixel>::LoadTGA(cstr fname)
{
	std::fstream img(fname, std::fstream::in | std::fstream::binary | std::fstream::ate);
	
	if(!img.is_open() || !img.tellg())
	{
		return false; // Error, file could not be opened or it's empty
	}
	
	char b;
	TgaHeader head;
	img.seekg(0); // Return to the beginning of a file

	while(img.get(b))
	{
		switch((int)img.tellg()-1)
		{
		case 2:
			head.image_type = b;
			break;

		case 12:
		{
			byte low_byte = b; // Save low order byte of width (little-endian)
			img.get(b); // Move 1 byte forward, to high order byte
			int high_byte = (byte)b;
			head.width = (high_byte << 8) + low_byte; // Combine 2 bytes to form width

			img.get(b); // Move on to height bytes
			low_byte = b;
			img.get(b);
			high_byte = (byte)b;
			head.height = (high_byte << 8) + low_byte; // Combine 2 bytes to form height

			img.get(b); // Move on to "bits per pixel" byte
			head.bpp = b;
			break;
		}

		case 18:
			if(head.width != width_ || head.height != height_ || head.bpp != 32)
			{
				return false; // Exit function, incorrect image dimentions or bpp
			}
			
			switch(head.image_type)
			{
			case 2: // Unmapped (raw) RGB image
				for(int i = 0; i < size_; ++i)
				{
					data[i].b = charToFloat(b);
					img.get(b); // Move to green
					data[i].g = charToFloat(b);
					img.get(b); // Move to red
					data[i].r = charToFloat(b);
					img.get(b); // Move to alpha
					data[i].a = charToFloat(b);
					img.get(b); // Move to next pixel
				}
				break;

			case 10: // Run Length Encoded (RLE) RGB image
				for(int i = 0; i < size_; ++i)
				{
					const byte mask_id = 128;
					const byte mask_length = 127;
					const byte packet = b; // Packet header
					const byte pl = (packet & mask_length) + 1; // Packet length
					const int pix_to_fill = i + pl; // bound on how much pixels to fill

					if(packet & mask_id) // Run-length packet
					{
						SciPixel run_color;
						img.get(b); // Move to color bytes
						run_color.b = charToFloat(b);
						img.get(b); // Move to green
						run_color.g = charToFloat(b);
						img.get(b); // Move to red
						run_color.r = charToFloat(b);
						img.get(b); // Move to alpha
						run_color.a = charToFloat(b);

						for(int j = i; j < pix_to_fill; ++j)
						{
							// Set run-color to all pixels of packet length
							data[j] = run_color;
							++i;
						}
						--i;

						img.get(b); // Move to next packet's header byte
					}
					else // Raw packet
					{
						for(int j = i; j < pix_to_fill; ++j)
						{
							img.get(b); // Move to pixel data (or next color)
							data[j].b = charToFloat(b);
							img.get(b); // Move to green
							data[j].g = charToFloat(b);
							img.get(b); // Move to red
							data[j].r = charToFloat(b);
							img.get(b); // Move to alpha
							data[j].a = charToFloat(b);

							++i;
						}
						--i;

						img.get(b); // Move to next packet's header byte
					}
				}
				break;

			default:
				return false; // Exit function, unsupported TARGA file type
				break;
			}
			break;

		default:
			break;
		}
	}

	return true; // Image successfully loaded
}
// End of Private functions
