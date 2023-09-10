// CONSOLE
#include <sddb>
#include <stdp>
#include <filep>
#include <filer>
#include <wtxta>
#include <ui64a>
#include <conint>

//wtxt bdir = WL("D:\\hw\\holywars_local_fixed_tst3\\holywars.local");
//wtxt bdir = WL("D:\\hw\\local.test");
//wtxt bdir = WL("D:\\hw\\holywars.local");
wtxt bdir = WL("D:\\hw\\holywars.offline");
//wtxt bdir = WL("D:\\hw\\websites");
//wtxt bdir = WL("D:\\vbox\\test.local");
//wtxt bdir = L"D:\\vbox\\holywars.local";


////// make programme quick title searche holyware + title+descript + IN EVERYTHING based on NPPFIF case insnsetive!

#define INC_TYPES_S 1
wtxt inc_types[INC_TYPES_S] = {
	WL(".html")
	//L".htm"
};

wtxta files = 65536;
ui64a fdepth = 65536;

wtxta broken_files = 128;
/////////////////////////////////////////////////////
//wtxta good_files = 128;
//////////////////////////////

ui64 dir_depth;
ui64 file_depth;

wtxta dirs_up = 16384;		// Upper level of directory tree
wtxta dirs_low = 16384;		// Lower level of directory tree

#define BUF_SIZE 0xA00000	// 10 485 760 B
txt src = BUF_SIZE;

void scan(const wtxt &dir);
void findAndReplace(const wtxt &file);

i32 wmain() 
{
	//p|"PAUSE!"|P|N;
	p|DC|N|N;
	
	dirs_up << bdir;
	
	SWSET
	while(~dirs_up != 0)
	{
		for(ui64 i = 0; i < ~dirs_up; ++i)
		{
			scan(dirs_up[i]);
		}
		
		dirs_low.MoveInto(dirs_up);
		++dir_depth;
	}
	SWSTOP
	
	p|~files|CR;
	p|N;
	SWPRINT
	SWRESET
	
	SWSET
	for(ui64 i = 0; i < ~files; ++i)
	{
		//if(i < 17500)////////////////////////////////////////
		//{
		//	continue;
		//}
		
		
		//if(files[i] != WL("D:\\hw\\holywars.local\\comments\\12949\\index.html"))
		//{
		//	continue;
		//}
		
		file_depth = fdepth[i];
		
		///////////////////////////////////////////////////////
		//if(file_depth > 0)
		//{
		//	printProgBar(~files, i+1, 0, 0, 0);
		//	continue;
		//}
		
		findAndReplace(files[i]);
		
		if(i % 128 == 0)
		{
			printProgBar(~files, i, 0, 0, 0);
		}
	}
	SWSTOP
	
	printProgBar(~files, ~files, 0, 0, 0);
	
	SWPRINT
	
	p|N|"BROKEN FILES"|N;
	broken_files.Print();
	//p|N|"GOOD FILES"|N;
	
	
	
	//wtxta very_good_files = 128;
	//ui64a fids = 128;
	//ui64a tss = 128;
	//for(ui64 i = 0; i < ~good_files; ++i)
	//{
	//	ui64 ts_st = txtf(good_files[i], 0, WL(".ru\\")) + 4;
	//	if(good_files[i][ts_st] != '2')
	//	{
	//		continue;
	//	}
	//	if(txtf(good_files[i], 0, WL("war\\all")) != NFND)
	//	{
	//		continue;
	//	}
	//	
	//	ui64 id_st = txtf(good_files[i], ts_st, WL("ts\\")) + 3;
	//	ui64 id_ed = txtf(good_files[i], id_st, '\\') - 1;
	//	
	//	ui64 id = t2i(txtsp(good_files[i], id_st, id_ed));
	//	ui64 ts = t2i(txts(good_files[i], ts_st, 14));
	//	
	//	bool64 found = false;
	//	ui64 j = 0;
	//	for(; j < ~fids; ++j)
	//	{
	//		if(id == fids[j])
	//		{
	//			found = true;
	//			break;
	//		}
	//	}
	//	
	//	if(found)
	//	{
	//		if(ts > tss[j])
	//		{
	//			very_good_files[j] = good_files[i];
	//			tss[j] = ts;
	//		}
	//	}
	//	else
	//	{
	//		very_good_files << good_files[i];
	//		fids << id;
	//		tss << ts;
	//	}
	//}
	//
	//very_good_files.Print();
	
	
	//D:\hw\websites\holywars.ru\20090420024938\comments\88\index.html
	
	
	
	
	
	
	//good_files.Print();
	
	//filep tp = "DIR_DEPTHS.TXT";
	//tp|TRN;
	//
	//for(ui64 i = 0; i < ~files; ++i)
	//{
	//	tp|fdepth[i]|S|S|S|S|files[i]|N;
	//}
	
	p|TB|P;
	return 0;
}

void getFileExt(const wtxt &fn, wtxt &ext)
{
	ui64 dot = txtfe(fn, TEND, '.');
	
	txtclr(ext);
	if(dot == NFND)
	{
		return;
	}
	
	ui64 sz = (~fn - dot);
	mcpy(ext, (const wchar_t *)fn + dot, sz * sizeof(wchar_t));
	txtssz(ext, sz);
	t2low(ext);
}

wtxt star = WL("\\*");
wtxt fnd_all = MAX_PATH;
wtxt full_fn = MAX_PATH;
wtxt ign_dir = MAX_PATH;
wtxt fext = MAX_PATH;
wtxt fn;

void scan(const wtxt &dir)
{	
	HANDLE ffind;
	WIN32_FIND_DATAW fdata;
	
	fnd_all += dir, fnd_all += star;
	
	// Start files enumeration =================================================================
	ffind = FindFirstFileExW(
		fnd_all,					// [I] LPCSTR Full			dir + path
		FindExInfoBasic,			// [I] FINDEX_INFO_LEVELS	Search accuracy
		&fdata,						// [O] LPVOID				Ouput file data
		FindExSearchNameMatch,		// [I] FINDEX_SEARCH_OPS	Non wildcard filters
		NULL,						// [I] LPVOID				Must be NULL (no support)
		FIND_FIRST_EX_LARGE_FETCH);	// [I] DWORD				Additional search flags
	// =========================================================================================
	
	if(ffind == INVALID_HANDLE_VALUE)
	{
		p|PE|" In FindFirstFileExA!"|N;
		return;
	}

	do
	{
		txtsdt(fn, MAX_PATH, 0, fdata.cFileName); // Always set direct mianpulated text back to 0!
		txtszu(fn);
		
		if((~fn == 1 && fn[0] == '.') || (~fn == 2 && fn[0] == '.' && fn[1] == '.'))
		{
			continue;
		}
		
		full_fn += dir, full_fn += '\\', full_fn += fn;
		
		if(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			dirs_low << full_fn;
		}
		else
		{
			getFileExt(fn, fext);
			
			bool64 to_skip = true;
			for(ui64 i = 0; i < INC_TYPES_S; ++i) // Include only selected file types
			{
				if(fext == inc_types[i])
				{
					to_skip = false;
					break;
				}
			}
			
			
			
			//////////// TMP FILTER OF _OUT FILES!
			//if(txtf(fn, 0, WL("_out.html")) != NFND)
			//{
			//	to_skip = true;
			//}
			
			
			
			
			if(!to_skip)
			{
				files << full_fn;
				fdepth << dir_depth;
			}
		}
		
		if(~files % 48 == 0)
		{
			p|~files|CR;
		}
		
		txtclr(full_fn);
		txtclr(fext);
	}
	while(FindNextFileW(ffind, &fdata));
	FindClose(ffind);
	
	txtclr(fnd_all);
	txtsdt(fn, 0, 0, NULL); // Directly mianpulated text set back to 0 before it goes out of scope
}

ui64a shifts = 256;
ui64a rel_shifts = 256;
txt fresult = 1024;
txt file_end = 512;

const txt two_dots = L("..");
const txt two_dots_slh = L("../");
const txt index_html = L("index.html");
txt index_html_final = 64;
txt depth_dots = 32;

void depth2dots(ui64 depth)
{
	txtclr(depth_dots);
	for(ui64 i = depth; i > 0; --i)
	{
		if(i == 1)
		{
			depth_dots += two_dots;
		}
		else
		{
			depth_dots += two_dots_slh;
		}
	}
}

void repQuestMarks(ui64 lnks, ui64 lnke) // Replace all ? with _
{
	for(ui64 i = lnks; i < lnke; ++i)
	{
		if(src[i] == '?')
		{
			src[i] = '_';
		}
	}
}

void insertIndexHtml(ui64 lnks, ui64 lnke)
{
	ui64 dot = txtfr(src, lnks, lnke, '.');
	if(dot == NFND) // Reletive link is pointing to a webpage, not an image
	{
		if(src[lnke-1] != '/')
		{
			index_html_final += '/';
		}
		
		index_html_final += index_html;
		txti(src, lnke, index_html_final);
		txtclr(index_html_final);
	}
}

const txt root_site_url = L("holywars.ru");
const txt root_site_full_url = L("http://holywars.ru");

void localiseLink(ui64 lnks)
{
	ui64 lnke = txtf(src, lnks, src[lnks-1]);
	if(src[lnks] == '/') // Relative link
	{
		repQuestMarks(lnks, lnke);
		insertIndexHtml(lnks, lnke);
		
		if(file_depth > 0) // Not in a root folder
		{
			txti(src, lnks, depth_dots);
		}
		else
		{
			txtd(src, lnks, 1);
		}
	}
	else // Ouside world wide web link
	{
		ui64 domain_st = txtfr(src, lnks, lnke, L("//")) + 2;
		if(domain_st != NFND && txtseq(src, domain_st, root_site_url)) // Root site must be also turned to rel. link
		{
			ui64 domain_end = txtf(src, domain_st, '/')-1;
			repQuestMarks(lnks, lnke);
			insertIndexHtml(lnks, lnke);
			txtrp(src, lnks, domain_end, depth_dots);
		}
	}
}

const txt a_content = L("content");
const txt a_href = L("href");
const txt a_src = L("src");
const txt a_class = L("class");
const txt a_title = L("title");

const txt html_end_rn = L("</body>\r\n</html>\r\n");
const txt html_html_beg = L("<html");
const txt html_body_end = L("</body>");
const txt html_html_end = L("</html>");

void processTagAttribute(ui64 tag_name_ed, const txt &attr) // NOTE WILL FAIL IF ATTRIBUTE IS EMPTY
{
	ui64 tag_ed = txtf(src, tag_name_ed, '>');
	ui64 attr_st = txtfr(src, tag_name_ed, tag_ed, attr);
	if(attr_st != NFND)
	{
		localiseLink(txtfa(src, attr_st + ~attr, L("'\"")) + 1); // Stupid HTML and it's '/" abiguity
	}
}

txt attr_value = 1024;

const txt & attrVal(ui64 tag_name_ed, const txt &attr)
{
	ui64 tag_ed = txtf(src, tag_name_ed, '>');
	ui64 attr_st = txtfr(src, tag_name_ed, tag_ed, attr);
	
	if(attr_st == NFND)
	{
		txtclr(attr_value);
		return attr_value;
	}
	
	ui64 val_st = txtfa(src, attr_st + ~attr, L("'\"")) + 1;
	ui64 val_ed = txtf(src, val_st, src[val_st-1]) - 1;
	txtsp(attr_value, src, val_st, val_ed);
	return attr_value;
}

wtxt user_id = 32;
txt user_id_u8 = 32;

txt his_comm = L("Его комментарии");
txt her_comm = L("Её комментарии");

txt his_her_comm_val = 64;
txt comments_link = 256;
txt img_link = 512;
txt hex = 16;

filer fr;
filep fp;
/////////////////////////////////////////////////////////////////////////////////////////////////
#pragma warning( disable : 4702 ) //TMP/////////////////////////////////////////////////////////
void findAndReplace(const wtxt &file)
{
	depth2dots(file_depth);
	
	fr < FO(file) > src > FC;
	
	if(~src < 69)
	{
		return;
	}
	
	////////////////// CHECK IF THE HTML FILE IS BROKEN
	if(txtf(src, 0, html_html_beg) == NFND)
	{
		broken_files << file;
		return;
	}
	ui64 body_end = txtfe(src, TEND, html_body_end);
	if(body_end == NFND || txtf(src, body_end + ~html_body_end, html_html_end) == NFND)
	{
		broken_files << file;
		return;
	}
	////////////////// CHECK IF THE HTML FILE IS BROKEN
	
	
	// MAKING FULL OFFLINE LINKS /////////////////////////////////////
	/* ELSE -> HANDLE OUSIDE WWW LINKS HERE!
	!!!!!!!!!!!!!!!!
	 FOR <a></a> ONLY RELETIVE LINKS MUST BE CHANGED
	!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	<tr><td class="n"><a href="1237403323.33.jpg">1237403323.33.jpg</a></td><td class="m">2010-Nov-19 14:35:59</td><td class="s">7.2K</td><td class="t">image/jpeg</td></tr> IF THERE IS NOT HTTP/HTTPS DO NOT TOUCH THE LINK!
	
	href="#comment_484811" skip!!!
	
	// INCLUDE NOT ONLY <IMG> TAG BUT <A> TOO!
	D:\hw\holywars.offline\comments\506\index.html
	<a rel="nofollow" href="http://images.eurogamer.net/assets/articles//a/7/5/1/4/3/0/pc_1080p_000.bmp.jpg" target="_blank">
	images.eurogamer.net/assets/articles//a/ ...</a> pc <br/>
	<br/>
	<a rel="nofollow" href="http://images.eurogamer.net/assets/articles//a/7/5/1/4/3/0/360_1080p_000.bmp.jpg" target="_blank">
	images.eurogamer.net/assets/articles//a/ ...</a> ящик <br/>*/
	
	
	for(ui64 i = 0; i < ~src - 4; ++i)
	{
		if(src[i] == '<' && src[i+1] == 'i' && src[i+2] == 'm' && src[i+3] == 'g')
		{
			ui64 img_ed = txtf(src, i+4, '>');
			ui64 src_st = txtfr(src, i+4, img_ed, a_src);
			ui64 lnk_st = txtfa(src, src_st, L("'\"")) + 1;
			
			if(txtsneq(src, lnk_st, L("http"))) // Skips all internal links
			{
				i = img_ed;
				continue;
			}
			
			ui64 lnk_ed = txtfr(src, lnk_st, img_ed, src[lnk_st-1]) - 1;
			
			txtsp(img_link, src, lnk_st, lnk_ed);
			
			ui64 after_ext = txtfci(img_link, 0, L(".jpg"));
			if(after_ext == NFND)
			{
				after_ext = txtfci(img_link, 0, L(".png"));
				if(after_ext == NFND)
				{
					after_ext = txtfci(img_link, 0, L(".gif"));
					if(after_ext == NFND)
					{
						after_ext = txtfci(img_link, 0, L(".jpeg"));
						if(after_ext == NFND)
						{
							after_ext = txtfci(img_link, 0, L(".bmp"));
							if(after_ext == NFND)
							{
								i = img_ed;
								continue;
							}
							else
							{
								after_ext += 4;
							}
						}
						else
						{
							after_ext += 5;
						}
					}
					else
					{
						after_ext += 4;
					}
				}
				else
				{
					after_ext += 4;
				}
			}
			else
			{
				after_ext += 4;
			}
			
			if(after_ext != ~img_link)
			{
				txtd(img_link, after_ext, TEND);
			}
			
			for(ui64 j = 0; j < ~img_link; ++j)
			{
				if(img_link[j] == '%') // Uniform Resource Identifier (URI) to Internationalized Resource Identifier (IRI)
				{
					txtclr(hex);
					txtssz(hex, 2);
					hex[0] = img_link[j+1];
					hex[1] = img_link[j+2];
					txtr(img_link, j, 3, (char)h2i(hex));
				}
			}
			
			ui64 http_ed = txtf(img_link, 0, L("//")); // This slashes will be spared to serve as the local URL
			txti(img_link, http_ed+1, L("web")); // http://foo.com/img.jpg ---> http:/web/foo.com/img.jpg
			txtr(img_link, 0, http_ed, depth_dots); // http:/web/foo.com/img.jpg ---> ../../web/foo.com/img.jpg
			
			txtrp(src, lnk_st, lnk_ed, img_link);
			i = img_ed; // Image tag end is now at different position after replace operation, but this will still save some search cycles
		}
		else if(src[i] == '<' && src[i+1] == 'a')
		{
			ui64 anchor_ed = txtf(src, i+4, '>');
			ui64 href_st = txtfr(src, i+4, anchor_ed, a_href);
			ui64 lnk_st = txtfa(src, href_st, L("'\"")) + 1;
			
			if(txtsneq(src, lnk_st, L("http"))) // Skips all internal links
			{
				i = anchor_ed;
				continue;
			}
			
			ui64 lnk_ed = txtfr(src, lnk_st, anchor_ed, src[lnk_st-1]) - 1;
			
			txtsp(img_link, src, lnk_st, lnk_ed);
			
			ui64 after_ext = txtfci(img_link, 0, L(".jpg"));
			if(after_ext == NFND)
			{
				after_ext = txtfci(img_link, 0, L(".png"));
				if(after_ext == NFND)
				{
					after_ext = txtfci(img_link, 0, L(".gif"));
					if(after_ext == NFND)
					{
						after_ext = txtfci(img_link, 0, L(".jpeg"));
						if(after_ext == NFND)
						{
							after_ext = txtfci(img_link, 0, L(".bmp"));
							if(after_ext == NFND)
							{
								i = anchor_ed;
								continue;
							}
							else
							{
								after_ext += 4;
							}
						}
						else
						{
							after_ext += 5;
						}
					}
					else
					{
						after_ext += 4;
					}
				}
				else
				{
					after_ext += 4;
				}
			}
			else
			{
				after_ext += 4;
			}
			
			if(after_ext != ~img_link)
			{
				txtd(img_link, after_ext, TEND);
			}
			
			for(ui64 j = 0; j < ~img_link; ++j)
			{
				if(img_link[j] == '%')
				{
					txtclr(hex);
					txtssz(hex, 2);
					hex[0] = img_link[j+1];
					hex[1] = img_link[j+2];
					txtr(img_link, j, 3, (char)h2i(hex));
				}
			}
			
			ui64 http_ed = txtf(img_link, 0, L("//"));
			txti(img_link, http_ed+1, L("web"));
			txtr(img_link, 0, http_ed, depth_dots);
			
			txtrp(src, lnk_st, lnk_ed, img_link);
			i = anchor_ed;
		}
	}
	
	return;
	
	//goto save_file;
	
	
	//good_files << file;
	//return;
	
	
	/* HOLYWARS SPECIFIC AFTER FIXES
	<span class="name" title="ScienceDiscoverer">ScienceDisco…</span> <<<< SINGLE CHAR ELLIPSIS!@!!!
	<span class="name" title="ScienceDiscoverer">ScienceDisco&#133;</span> <<<< ENTITY
	<span class="name" title="ScienceDiscoverer">ScienceDiscoverer</span>
	
	Её комментарии (562) Его комментарии (36780) ----> LINKS ADD ON ALL ROOT USER INDEX.HTML + USER/WARS/INDEX.HTML
	<a href="../../user/3943/wars/index.html">Его холивары (65)</a> | 
	<span>Его комментарии (18220)</span>
	<span>Её комментарии (18220)</span>
	D:\hw\holywars.local\user\15\index.html
	D:\hw\holywars.local\user\15\wars\index.html
	
	<a href="../../../comments/16767#comment_914382/index.html" title="ссылка на комментарий">
	<a href="../../../comments/16767/index.html#comment_914382" title="ссылка на комментарий">
	
	DELETE THIS (725 WARS WAS POLLUTED WITH THIS AD!):
	<a title="YouTube канал 8cinq" href="https://www.youtube.com/c/8cinq" target="_blank ">
	<img src="../../public/images/yourube_pic.gif" style="width: 196px; height: 303px; border: 0;"></a>
	
	REMOVE YANDEX METRIKA
	<!-- Yandex.Metrika counter -->
	.....................
	<!-- /Yandex.Metrika counter -->
	
	FIX ALL INTERNAL LINKS
	<a class="internal" href="http://holywars.ru/comments/720" target="_blank">Ссылка</a>
	<a class="internal" href="../../comments/720" target="_blank">Ссылка</a>
	<a class="internal" href="../../comments/720/index.html" target="_blank">Ссылка</a>
	<a class="internal" href="../../comments/9937#comment_401584" target="_blank">holywars.ru/comments/9937#comment_401584</a>
	<a class="internal" href="../../comments/9937/index.html#comment_401584" target="_blank">holywars.ru/comments/9937#comment_401584</a>
	<a class="menuitem" href="../../comments/340">
	<a href="../../../../../comments/7756" target="_blank">
	<a href=&quot;../../comments/3557&quot;>
	<a class="menuitem" href="/comments/9344">Холивар самоуправления</a> ---> Home Page
	*/
	
	for(ui64 i = 0; i < ~src - 5; ++i) // Max HTML tag -> [<span] -> 5 characters, acceptable approximation
	{
		const char *t = (const char *)src + i;
		if(memeq(t, "<span", 5))
		{
			ui64 tag_name_ed = i + 5;
			if(src[tag_name_ed] != '>')  // Span with attributes
			{
				if(attrVal(tag_name_ed, a_class) != L("name"))
				{
					i = tag_name_ed;
					continue;
				}
				
				ui64 span_val_st = txtf(src, tag_name_ed, '>') + 1;
				ui64 span_val_ed = txtf(src, span_val_st, '<') - 1;
				if(txtfr(src, span_val_st, span_val_ed, L("&#133;")) != NFND)
				{
					txtrp(src, span_val_st, span_val_ed, attrVal(tag_name_ed, a_title));
				}
			}
			else // No-attribute span
			{
				if(txtseq(file, ~bdir, WL("\\user\\")) && txtf(file, 0, WL("comments")) == NFND)
				{
					ui64 span_ed = txtf(src, tag_name_ed, L("</span>")) + 6;
					if(txtfr(src, tag_name_ed, span_ed, his_comm) == NFND && txtfr(src, tag_name_ed, span_ed, her_comm) == NFND)
					{
						continue;
					}
					
					ui64 user_id_ed = txtf(file, ~bdir + 6, '\\') - 1;
					txtsp(user_id, file, ~bdir + 6, user_id_ed);
					wt2u8(user_id_u8, user_id);
					
					txtsp(his_her_comm_val, src, i + 6, span_ed - 7);
					
					if(txtf(file, 0, WL("\\wars\\")) == NFND) // D:\hw\holywars.local\user\15\index.html
					{
						comments_link += L("<a href=\"../../user/");
					}
					else // D:\hw\holywars.local\user\15\wars\index.html
					{
						comments_link += L("<a href=\"../../../user/");
					}
					
					comments_link += user_id_u8;
					comments_link += L("/comments/index.html\">");
					comments_link += his_her_comm_val;
					comments_link += L("</a>");
					
					txtrp(src, i, span_ed, comments_link);
					txtclr(comments_link);
					i += ~comments_link - 1;
				}
			}
		}
		else if(src[i] == '<' && src[i+1] == 'a')
		{
			ui64 anchor_ed = txtf(src, i+2, '>');
			
			if(attrVal(i+2, a_title) == L("YouTube канал 8cinq"))
			{
				ui64 anchor_full_ed = txtf(src, anchor_ed+1, L("</a>")) + 3;
				txtdp(src, i, anchor_full_ed);
				continue;
			}
			
			if(txtfr(src, i+2, anchor_ed, L("../comments/")) != NFND) // Home Page /comments/ must be fixed manually
			{
				if(txtfr(src, i+2, anchor_ed, L("/index.html")) == NFND)
				{
					ui64 href_st = txtfr(src, i+2, anchor_ed, a_href);
					
					ui64 lnk_st = txtfr(src, href_st, anchor_ed, L("&quot;"));
					if(lnk_st == NFND)
					{
						lnk_st = txtfa(src, href_st, L("'\"")) + 1;
					}
					else
					{
						lnk_st += L("&quot;").s;
					}
					
					ui64 ins_pos = txtfa(src, lnk_st, L("'\"#&"));
					txti(src, ins_pos, L("/index.html"));
				}
				i = anchor_ed;
				continue;
			}
			
			if(txtf(file, 0, WL("comments")) != NFND)
			{
				if(attrVal(i+2, a_title) != L("ссылка на комментарий"))
				{
					continue;
				}
				
				ui64 link_st = txtfr(src, i+2, anchor_ed, L("../../../comments/"));
				if(link_st != NFND)
				{
					txti(src, txtfr(src, link_st, anchor_ed, '#'), L("/index.html"));
					anchor_ed = txtf(src, i+2, '>');
					ui64 del_index_st = txtfe(src, anchor_ed, L("/index.html"));
					txtd(src, del_index_st, L("/index.html").s);
				}
			}
		}
		
		if(memeq(t, "<!-- Yandex.Metrika counter -->", 31))
		{
			txtdp(src, i, txtf(src, i + 31, L("<!-- /Yandex.Metrika counter -->")) + L("<!-- /Yandex.Metrika counter -->").s - 1);
		}
	}
	
	return;
	
	goto save_file;
	
///////////////////////////////////////////////////////////////////////////////////////////////////
//<meta property="og:image" content="http://holywars.ru/public/upload/war/pic2_left.gif" />
//<meta property="og:image" content="../../../holywars.ru/public/upload/war/pic2_left.gif" />
//
//<link href="/public/cache/css/bundle_54c4c1ecb9a390d2b35d095dfe6f513a.css?1481146824" media="screen" rel="stylesheet" type="text/css" />
//<link href="../../public/cache/css/bundle_54c4c1ecb9a390d2b35d095dfe6f513a0a35.css" media="screen" rel="stylesheet" type="text/css" />
//
//<script type="text/javascript" src="/public/cache/js/bundle_35e2541121f08b77dcae7497569df462.js?1477255609"></script>
//<script type="text/javascript" src="../../public/cache/js/bundle_35e2541121f08b77dcae7497569df4627a8b.js"></script>
//D:\vbox\holywars.local\public\cache\js\bundle_35e2541121f08b77dcae7497569df462.js_1477255609
//
//<a class="menuitem" href="/about">О чем этот сайт?</a>
//<a class="menuitem" href="../../about/index.html">О чем этот сайт?</a> 
//<a href="/user/4114" style="font-size:12px;">G.Wox</a>
//<a href="../../user/4114/index.html" style="font-size:12px;">G.Wox</a>
//// FOR <a></a> ONLY RELETIVE LINKS MUST BE CHANGED
//
//<img src="/public/images/section_help.gif" alt="Справка по разделам" width="16" height="16" style="position:relative; right:100px; top:5px; border:0px;"/>
//<img src="../../public/images/section_help.gif" alt="Справка по разделам" width="16" height="16" style="position:relative; right:100px; top:5px; border:0px;"/>
//
//<td class="warname_right_column with_pic" align="right" style="background-image: url('../../public/upload/war/1340454142.201.jpg')"  property="schema:image" resource="../../../holywars.ru/public/upload/war/1340454142.201.jpg">
//<td class="warname_right_column with_pic" align="right" style="background-image: url('/public/upload/war/1340454142.201.jpg')"  property="schema:image" resource="http://holywars.ru/public/upload/war/1340454142.201.jpg">
//
//background-image:url('../../public/images/module_top_pop1.gif')
//
//<div role="navigation" title="Холивары недели" class="outdiv" style="width:194px; padding-top: 46px; background-image:url('/public/images/module_top_pop1.gif'); background-repeat: no-repeat">
//<div role="navigation" title="Холивары недели" class="outdiv" style="width:194px; padding-top: 46px; background-image:url('../../public/images/module_top_pop1.gif'); background-repeat: no-repeat">
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	for(ui64 i = 0; i < ~src - 7; ++i) // Max HTML tag -> [<script] -> 7 characters, acceptable approximation
	{
		const char *t = (const char *)src + i;
		if(memeq(t, "<meta", 5)) // <meta content="http://...." />
		{
			processTagAttribute(i + 5, a_content);
		}
		else if(memeq(t, "<link", 5)) // <link href="/public/cache/...6824"
		{
			processTagAttribute(i + 5, a_href);
		}
		else if(memeq(t, "<script", 7)) // <script type="text/javascript" src="/public/...09"
		{
			processTagAttribute(i + 7, a_src);
		}
		else if(src[i] == '<' && src[i+1] == 'a') // <a class="menuitem" href="/about"></a>
		{
			processTagAttribute(i + 2, a_href);
		}
		else if(src[i] == '<' && src[i+1] == 'i' && src[i+2] == 'm' && src[i+3] == 'g') // <img src="/pub...p.gif"/>
		{
			processTagAttribute(i + 4, a_src);
		}
		else if(memeq(t, root_site_full_url, ~root_site_full_url))
		{
			localiseLink(i); // resource="http://holywars.ru/public/upload/war/pic2_left.gif"
		}
		
		if(src[i] == 'u' && src[i+1] == 'r' && src[i+2] == 'l' && src[i+3] == '(')
		{
			if(src[i+4] == '\'') // background-image: url('/public/upload/war/1340454142.201.jpg')"
			{
				localiseLink(i + 5);
			}
			else // background-image: url(/public/images/regaback.png);
			{
				localiseLink(i + 4);
			}
		}
	}
	
	
	// HOW IS THIS GENERATED??
	//<a href="/comments/1#comment_412" title="ссылка на комментарий" style="margin-left:8px">
	//<img class="permalink_wite" src="/public/images/permalink_white.gif"></a>
	// IN PUBLIC/JS/COMMENTS.JS!
	
	
	///////////////////////////////////////
	//wtxt tmp_out_file = file;
	//txti(tmp_out_file, txtfe(tmp_out_file, TEND, '.'), WL("_out"));
	//fp|FO(tmp_out_file)|TRN|src|FC;

//////////////////////// TMP JUMP
save_file:
	
	fp|FO(file)|TRN|src|FC;
}

//bool64 notHtmlChar(char c)
//{
//	return
//	c != '$'				&&
//	c != '-'				&&
//	c != '_'				&&
//	c != '.'				&&
//	c != '+'				&&
//	c != '!'				&&
//	c != '*'				&&
//	c != '\''				&&
//	c != '('				&&
//	c != ')'				&&
//	c != '/'				&&
//	c != '&'				&&
//	c != '?'				&&
//	c != '='				&&
//	c != ':'				&&
//	c != '%'				&&
//	(c < 'A' || c > 'Z')	&&
//	(c < 'a' || c > 'z')	&&
//	(c < '0' || c > '9');
//}