// CONSOLE
#define TXT_MULTI_THREAD_MODE
#include <stdp>
#include <stdr>
#include <txta>
#include <wtxta>
#include <ui64a>
#include <filer>
#include <filep>

#include <curl.h>

txta linkf = 28859;

void loadLinksWithExt()
{
	linkf.Clear();
	filer fr = "D:\\vbox\\all_links.txt";
	txt lnk;
	fr > lnk > FC;
	
	ui64 file_s = NFND; // HTML file path start position
	ui64 l_beg = NFND; // Line beginning
	txt flinks = 4096;
	for(ui64 i = 0; i < ~lnk; ++i)
	{
		if(i + 2 < ~lnk && lnk[i] == 'D' && lnk[i+1] == ':' && lnk[i+2] == '\\')
		{
			file_s = i;
		}
		
		if(lnk[i] == '\n')
		{
			if(file_s != NFND)
			{
				linkf << flinks;
				if(flinks != empty)
				{
					txtclr(flinks);
				}
				flinks += txtsp(lnk, file_s, i);
				file_s = NFND;
				
				if(i + 1 < ~lnk)
				{
					l_beg = i + 1;
				}
				continue;
			}
			
			txt link = txtsp(lnk, l_beg, i);
			ui64 last_sl = txtfe(link, TEND, '/'); // Last slash
			ui64 dot = txtfr(link, last_sl, TEND, '.');
			
			if(dot != NFND)
			{
				flinks += link;
			}
			
			if(i + 1 < ~lnk)
			{
				l_beg = i + 1;
			}
		}
	}
}

bool64 isLetter(char c)
{
	if((c < 'A' || c > 'Z')	&& (c < 'a' || c > 'z'))
	{
		return false;
	}
	
	return true;
}

const txt e_jpg = L(".jpg");
const txt e_png = L(".png");
const txt e_gif = L(".gif");
const txt e_jpeg = L(".jpeg");
const txt e_bmp = L(".bmp");

bool64 isImage(const txt &ext)
{
	if(
	ext == e_jpg	||
	ext == e_png	||
	ext == e_gif	||
	ext == e_jpeg	||
	ext == e_bmp)
	{
		return true;
	}
	
	return false;
}

void loadImageLinks()
{
	linkf.Clear();
	filer fr = "D:\\vbox\\all_links.txt";
	txt lnk;
	fr > lnk > FC;
	
	ui64 file_s = NFND; // HTML file path start position
	ui64 l_beg = NFND; // Line beginning
	txt flinks = 4096;
	for(ui64 i = 0; i < ~lnk; ++i)
	{
		if(i + 2 < ~lnk && lnk[i] == 'D' && lnk[i+1] == ':' && lnk[i+2] == '\\')
		{
			file_s = i;
		}
		
		if(lnk[i] == '\n')
		{
			if(file_s != NFND)
			{
				linkf << flinks;
				if(flinks != empty)
				{
					txtclr(flinks);
				}
				flinks += txtsp(lnk, file_s, i);
				file_s = NFND;
				
				if(i + 1 < ~lnk)
				{
					l_beg = i + 1;
				}
				continue;
			}
			
			ui64 dot = txtfe(lnk, i, '.');
			
			ui64 ext_e = dot + 1;
			while(ext_e < ~lnk && isLetter(lnk[ext_e]))
			{
				++ext_e;
			}
			
			txt ext = txtsp(lnk, dot, ext_e-1);
			t2low(ext);
			txt link = txtsp(lnk, l_beg, i);
			
			if(isImage(ext))
			{
				flinks += link;
			}
			
			if(i + 1 < ~lnk)
			{
				l_beg = i + 1;
			}
		}
	}
	
	filep fp = "D:\\vbox\\all_img_links.txt";
	fp|TRN;
	for(ui64 i = 0; i < ~linkf; ++i)
	{
		p|L("        ")|CR|i|CR;
		fp|linkf[i];
	}
}

void analyseLinkTypes()
{
	txta exts = 1024;
	ui64a exts_n = 1024;
	
	filer fr = "D:\\vbox\\all_links_exts.txt";
	txt lnk;
	fr > lnk > FC;
	
	ui64 file_s = NFND; // HTML file path start position
	ui64 l_beg = NFND; // Line beginning
	for(ui64 i = 0; i < ~lnk; ++i)
	{
		if(i + 2 < ~lnk && lnk[i] == 'D' && lnk[i+1] == ':' && lnk[i+2] == '\\')
		{
			file_s = i;
		}
		
		if(lnk[i] == '\n')
		{
			if(file_s != NFND) // Just skip files
			{
				file_s = NFND;
				if(i + 1 < ~lnk)
				{
					l_beg = i + 1;
				}
				continue;
			}
			
			
			ui64 dot = txtfe(lnk, i, '.');
			txt ext = txtsp(lnk, dot, i-1);
			
			bool64 ext_fnd = false;
			for(ui64 j = 0; j < ~exts; ++j)
			{
				if(exts[j] == ext)
				{
					++exts_n[j];
					ext_fnd = true;
					break;
				}
			}
			if(!ext_fnd)
			{
				exts << ext;
				exts_n << 1;
			}
			
			if(i + 1 < ~lnk)
			{
				l_beg = i + 1;
			}
		}
	}
	
	ui64 tot_num = 0;
	for(ui64 i = 0; i < ~exts_n; ++i)
	{
		tot_num += exts_n[i];
	}
	
	for(ui64 i = 0; i < ~exts; ++i) // Insertion sort
	{
		for(ui64 j = i+1; j < ~exts; ++j)
		{
			if(exts_n[j] > exts_n[i])
			{
				ui64 tmp = exts_n[i];
				exts_n[i] = exts_n[j];
				exts_n[j] = tmp;
				
				txt tmpt = exts[i];
				exts[i] = exts[j];
				exts[j] = tmpt;
			}
		}
	}
	
	for(ui64 i = 0; i < ~exts; ++i)
	{
		txt perc = i2t((ui64)(((double)exts_n[i]/(double)tot_num) * 1000.0));
		txti(perc, ~perc-1, '.');
		
		p|SPN(15)|exts[i]|S|exts_n[i]|S|perc|'%'|N;
	}
	
	p|L("TOTAL LINKS: ")|tot_num|N;
}

bool64 fileExists(const char *fn)
{
	DWORD attr = GetFileAttributesA(fn);
	if(attr == INVALID_FILE_ATTRIBUTES)
	{
		return false;
	}
	
	return true;
}

const txt base_p = L("D:\\vbox\\holywars.ru.local\\holywars.ru\\");
const txt dsl = L("//");
void loadLinksNotDown() // Load only links not present in local mirror
{
	linkf.Clear();
	filer fr = "D:\\vbox\\all_img_links.txt";
	txt lnk;
	fr > lnk > FC;
	
	ui64 file_s = NFND; // HTML file path start position
	ui64 l_beg = NFND; // Line beginning
	txt flinks = 4096;
	txt hex = 16;
	for(ui64 i = 0; i < ~lnk; ++i)
	{
		if(i % 1024 == 0)
		{
			p|L("SCAN: ")|i|CR;
		}
		
		if(i + 2 < ~lnk && lnk[i] == 'D' && lnk[i+1] == ':' && lnk[i+2] == '\\')
		{
			file_s = i;
		}
		
		if(lnk[i] == '\n')
		{
			if(file_s != NFND)
			{
				linkf << flinks;
				if(flinks != empty)
				{
					txtclr(flinks);
				}
				flinks += txtsp(lnk, file_s, i);
				file_s = NFND;
				
				if(i + 1 < ~lnk)
				{
					l_beg = i + 1;
				}
				continue;
			}
			
			txt link_p = txtsp(lnk, l_beg, i-1); // Link converted to local path
			ui64 d_sl = txtf(link_p, 0, dsl); // Double forward slash (http: --> [//] <--)
			link_p = txtd(link_p, 0, d_sl+2);
			
			for(ui64 j = 0; j < ~link_p; ++j)
			{
				if(link_p[j] == '/')
				{
					link_p[j] = '\\';
				}
				
				if(link_p[j] == '%') // Uniform Resource Identifier (URI) to Internationalized Resource Identifier (IRI)
				{
					txtclr(hex);
					txtssz(hex, 2);
					hex[0] = link_p[j+1];
					hex[1] = link_p[j+2];
					txtr(link_p, j, 3, (char)h2i(hex));
				}
			}
			
			link_p = base_p + link_p;
			
//http://   holywars.ru/public/upload/war/1304347755.3294.jpg
//https://fc08.deviantart.net/fs71/f/2011/034/d/2/forever_a_pone_by_glamourkat-d38ooot.jpg
//D:\vbox\holywars.ru.local\holywars.ru\  www.razruha.org\data\media\4789   \1333919273f99.jpg
			
			if(!fileExists(link_p)) // Load only links missing from local mirror
			{
				txt link = txtsp(lnk, l_beg, i);
				flinks += link;
			}
			
			if(i + 1 < ~lnk)
			{
				l_beg = i + 1;
			}
		}
	}
	
	filep fp = "D:\\vbox\\all_miss_img_links.txt";
	fp|TRN;
	for(ui64 i = 0; i < ~linkf; ++i)
	{
		p|L("        ")|CR|i|CR;
		fp|linkf[i];
	}
}

ui64 loadLinks()
{
	//linkf.Clear();
	//filer fr = "FAILED_ZEROSIZE.TXT";
	////filer fr = L("D:\\vbox\\all_failed_links.txt");
	////filer fr = L("D:\\vbox\\all_miss_img_links.txt");
	////filer fr = L("D:\\P\\MT\\FAILED_REDIRECTS.TXT");
	//if(!fr)
	//{
	//	//fr < FO(L("all_failed_links.txt"));
	//	//fr < FO(L("all_miss_img_links.txt"));
	//	//fr < FO(L("FAILED_REDIRECTS.TXT"));
	//}
	//
	//txt lnk;
	//fr > lnk > FC;
	//
	//ui64 pure_links = 0;
	//ui64 file_s = NFND;		// HTML file path start position
	//ui64 l_beg = NFND;		// Line beginning
	//txt file_link = 4096;	// File or link
	//for(ui64 i = 0; i < ~lnk; ++i)
	//{
	//	if(i + 2 < ~lnk && lnk[i] == 'D' && lnk[i+1] == ':' && lnk[i+2] == '\\')
	//	{
	//		file_s = i;
	//	}
	//	
	//	if(lnk[i] == '\n')
	//	{
	//		if(file_s != NFND)
	//		{
	//			file_link = txtsp(lnk, file_s, i-1); // Do not include newline
	//			linkf << file_link;
	//			file_s = NFND;
	//			
	//			if(i + 1 < ~lnk)
	//			{
	//				l_beg = i + 1;
	//			}
	//			continue;
	//		}
	//		
	//		++pure_links;
	//		
	//		file_link = txtsp(lnk, l_beg, i-1); // Do not include newline
	//		linkf << file_link;
	//		
	//		if(i + 1 < ~lnk)
	//		{
	//			l_beg = i + 1;
	//		}
	//	}
	//}
	//
	//return pure_links;
	
	linkf.Clear();
	filer fr = "all_failed_links.txt";
	
	txt lnk;
	fr > lnk > FC;
	
	ui64 l_beg = 0;		// Line beginning
	txt file_link = 4096;	// File or link
	for(ui64 i = 0; i < ~lnk; ++i)
	{
		if(lnk[i] == '\n')
		{
			txtsp(file_link, lnk, l_beg, i-1); // Do not include newline
			linkf << file_link;
			
			if(i + 1 < ~lnk)
			{
				l_beg = i + 1;
			}
		}
	}
	
	return ~linkf;
}

#define MAX_THREADS 24
#define MAX_TRYS	4	// How many times downloader thread will attempt to download link
#define TOUT_DT		400 // How much ms to add after each retry

bool64 all_threads_stop = false;
ui64 starting_link_idx;

ui64a lstatus;				// Status of the link
txta failed_links = 28859;	// YOU HAVE FAILED ME THE LAST TIME!

HANDLE mutex_main, mutex_status, mutex_print, mutex_fails, mutex_thread_num;
ui64 threads_active;

void incSafe(ui64 *var)
{
	WaitForSingleObject(mutex_status, INFINITE); // CRITICAL SECTION START ==================
	++*var;
	ReleaseMutex(mutex_status); // CRITICAL SECTION END =====================================
}

void incThreads()
{
	WaitForSingleObject(mutex_thread_num, INFINITE); // CRITICAL SECTION START ==================
	++threads_active;
	ReleaseMutex(mutex_thread_num); // CRITICAL SECTION END =====================================
}

ui64 curl_error;
ui64 redirect_fails;
ui64 status_ok;			// 200
ui64 status_ok_other;	// 201 - 226
ui64 status_redirect;	// 300 - 308
ui64 status_bad_req;	// 400
ui64 status_unauth;		// 401
ui64 status_need_pay;	// 402
ui64 status_forbid;		// 403
ui64 status_not_fnd;	// 404
ui64 status_fail_other;	// 405 - 451
ui64 status_server_err;	// 500 - 511
ui64 status_unrecogn;	// 600 - INFINITY

void decThreads()
{
	WaitForSingleObject(mutex_thread_num, INFINITE); // CRITICAL SECTION START ==================
	--threads_active;
	if(threads_active == 0)
	{
		p|SCP(0, MAX_THREADS+3);
		p|N|L("CURL ERRORS:       ")|curl_error|N;
		p  |L("REDIRECT FAILS:    ")|redirect_fails|N;
		p  |L("STATUS OK:         ")|status_ok|N;
		p  |L("STATUS OK OTHER:   ")|status_ok_other|N;
		p  |L("STATUS REDIRECT:   ")|status_redirect|N;
		p  |L("STATUS BAD REQ:    ")|status_bad_req|N;
		p  |L("STATUS UNAUTH:     ")|status_unauth|N;
		p  |L("STATUS NEED PAY:   ")|status_need_pay|N;
		p  |L("STATUS FORBID:     ")|status_forbid|N;
		p  |L("STATUS NOT FND:    ")|status_not_fnd|N;
		p  |L("STATUS FAIL OTHER: ")|status_fail_other|N;
		p  |L("STATUS SERVER ERR: ")|status_server_err|N;
		p  |L("STATUS UNRECOGN:   ")|status_unrecogn|N;
		
		filep fp = "FAILED_LINKS.TXT";
		for(ui64 i = 0; i < ~failed_links; ++i)
		{
			fp|failed_links[i]|N;
		}
	}
	ReleaseMutex(mutex_thread_num); // CRITICAL SECTION END =====================================
}

void printSafe(ui64 y_pos, ui64 idx, const txt &t)
{
	// |Thread xx: | -> 11 symbols
	WaitForSingleObject(mutex_print, INFINITE); // CRITICAL SECTION START ==================
	p|SCP(11, y_pos)|S(109);
	p|SCP(11, y_pos)|SPN(5)|idx|S|txts(t, 0, 100)|L("...");
	ReleaseMutex(mutex_print); // CRITICAL SECTION END =====================================
}

ui64 NOTHROW PARAMNOUSE headFunc(char *buffer, ui64 size, ui64 nitems, void *userdata)
{
	txtadd(*((txt *)userdata), buffer, nitems);
	return nitems; // [nitems] - size of data / [size] always 1 / No \0
}

ui64 NOTHROW PARAMNOUSE writeFunc(char *ptr, ui64 size, ui64 nmemb, void *userdata)
{
	((binf *)userdata)->Add(ptr, nmemb);
	return nmemb; // [nmemb] - size of data / [size] always 1 / No \0
}

const txt https_txt = L("https");
const txt e_jpg_caps = L(".JPG");
const txt e_png_caps = L(".PNG");
const txt e_txt = L(".txt");

const txt head_location = L("location:");
const txt head_content = L("content-type:");
const txt head_cont_img = L("image/");
const txt not_an_image = L(".not_an_image");
const txt cont_unknown = L("UNKNOWN CONTENT TYPE");

const txt output_dir_name = L("IMAGES\\");
const txt url_dsl = L("//");

void incResponseCode(i32 code)
{
	switch(code)
	{
	case 200:
		incSafe(&status_ok);
		break;
	case 400:
		incSafe(&status_bad_req);
		break;
	case 401:
		incSafe(&status_unauth);
		break;
	case 402:
		incSafe(&status_need_pay);
		break;
	case 403:
		incSafe(&status_forbid);
		break;
	case 404:
		incSafe(&status_not_fnd);
		break;
	default:
		break;
	}
	
	if(code >= 201 && code < 300)
	{
		incSafe(&status_ok_other);
	}
	else if(code >= 300 && code < 400)
	{
		incSafe(&status_redirect);
	}
	else if(code >= 405 && code < 500)
	{
		incSafe(&status_fail_other);
	}
	else if(code >= 500 && code < 600)
	{
		incSafe(&status_server_err);
	}
	else
	{
		incSafe(&status_unrecogn);
	}
}

THREAD downloader(void *param)
{
	incThreads();
	
	CURL *curl = NULL;
	txt last_file = MAX_PATH;
	txt out_fname = MAX_PATH;
	txt link_orig = 512;
	txt link = 512;
	txt link_redir = 512;
	txt ext = 32;
	txt cont_type = 128;
	binf image = 0x100000;	// 1'048'576 B
	txt head = 0x1000;		// 4'096 B
	ui64 dot, ext_e;
	//ui64 i = 0;
	ui64 i = starting_link_idx;
	
get_next_link:
	
	bool64 link_grabbed = false;
	WaitForSingleObject(mutex_main, INFINITE); // CRITICAL SECTION START ==================
	for(; i < ~linkf; ++i)
	{
		if(linkf[i][0] == 'D') // Source file found
		{
			last_file = linkf[i];
			continue;
		}
		
		if(!lstatus[i]) // Link is availabe for download attempts
		{
			link = linkf[i];
			lstatus[i] = true; // Now this link is owned by this thread
			link_grabbed = true;
			break;
		}
	}
	ReleaseMutex(mutex_main); // CRITICAL SECTION END =====================================
	
	if(!link_grabbed || all_threads_stop) // No more free links available, abort!
	{
		printSafe((ui64)param, i, L("No more links found, aborting..."));
		curl_easy_cleanup(curl);
		decThreads();
		return 0;
	}
	
	link_orig = link;
	cont_type = cont_unknown;
	printSafe((ui64)param, i, link);
	
	curl = curl_easy_init();
	if(!curl)
	{
		p|"Thread #"|GetCurrentThreadId()|" failed to initialise CURL! Exiting...";
		return 1;
	}
	
	bool64 https_change = false; // Change from http to https was tried
	bool64 png_2_jpg = false; // Change from png to jpg/jpeg was tried

	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headFunc);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunc);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &image);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &head);
	
retry_download:
	
	image.Clear();
	txtclr(head);
	
	curl_easy_setopt(curl, CURLOPT_URL, (const char *)link);
	
	CURLcode res;
	ui64 trys = MAX_TRYS;
	ui64 to_sleep = 0;
	bool64 curl_err = false;
	i32 http_code = 0;
	
	while(--trys != UI64_MAX)
	{
		res = curl_easy_perform(curl);
		if(res != CURLE_OK)
		{
			curl_err = true;
			if(res == CURLE_OPERATION_TIMEDOUT)
			{
				goto total_fail;
			}
			goto end_loop_sleep;
		}
		
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		
		if(http_code < 400)
		{
			break;
		}
		
	end_loop_sleep:
		to_sleep += TOUT_DT;
		Sleep((DWORD)to_sleep);
	}
	
	if(http_code >= 300 && http_code < 400) // Handle redirect
	{
		ui64 loc = txtfci(head, 0, head_location);
		if(loc != NFND)
		{
			ui64 url_beg = loc + ~head_location;
			while(head[url_beg] == ' ')
			{
				++url_beg;
			}
			
			ui64 url_end = url_beg;
			while(head[url_end] != '\r' && head[url_end] != '\n')
			{
				++url_end;
			}
			
			link_redir = txtsp(head, url_beg, url_end-1);
			
			if(link_redir[0] == '/') // Relative link
			{
				ui64 urlb = txtf(link, 0, url_dsl) + 2;
				ui64 urle = txtf(link, urlb, '/') - 1;
				link_redir = txtsp(link, 0, urle) + link_redir;
			}
			if(link_redir == link) // Site redirects to the same adress infinitely
			{
				incSafe(&redirect_fails);
				goto total_fail;
			}
			
			link = link_redir;
			goto retry_download;
		}
	}
	
	// Extract image extention
	dot = txtfe(link, TEND, '.');
	if(dot == NFND) // This link or it's redirect is broken, skip it
	{
		goto total_fail;
	}
	ext_e = dot + 1;
	while(ext_e < ~link && isLetter(link[ext_e]))
	{
		++ext_e;
	}
	--ext_e;
	ext = txtsp(link, dot, ext_e);
	
	if(curl_err || http_code >= 400) // Attempt failed!
	{
		if(link[4] != 's' && !https_change)
		{
			https_change = true;
			txti(link, 4, 's');
			goto retry_download;
		}
		
		if(!png_2_jpg)
		{
			png_2_jpg = true;
			if(https_change)
			{
				https_change = false;
				txtd(link, 4, 1); // Revert to http
			}
			
			if(ext == e_png) // Only png files will be attempted as jpg-s
			{
				txtr(link, dot, ext_e, e_jpg);
				ext = e_jpg;
				goto retry_download;
			}
			else if(ext == e_png_caps)
			{
				txtr(link, dot, ext_e, e_jpg_caps);
				ext = e_jpg_caps;
				goto retry_download;
			}
		}
		
total_fail: // Total failure

		if(curl_err)
		{
			incSafe(&curl_error);
		}
		else
		{
			incResponseCode(http_code);
		}
		
		WaitForSingleObject(mutex_fails, INFINITE); // CRITICAL SECTION START ==================
		failed_links << link_orig;
		ReleaseMutex(mutex_fails); // CRITICAL SECTION END =====================================
		goto get_next_link;
	}
	
	// Confirm that what was downloaded is actually an image
	ui64 cont = txtfci(head, 0, head_content);
	ui64 type_img = NFND;
	if(cont != NFND)
	{
		ui64 hend = txtf(head, cont, '\n');
		type_img = txtfr(head, cont, hend, head_cont_img);
		cont_type = txtsp(head, cont, hend-2); // -2 for \r\n
	}
	
	if(type_img == NFND)
	{
		ext += not_an_image;
	}
	
	// Image was successfully acquired
	
	CreateDirectoryA(output_dir_name, NULL);
	
	out_fname = i2t(i);
	filep fp = output_dir_name + out_fname + ext;
	fp|image|FC;
	fp|FO(output_dir_name + out_fname + e_txt);
	fp|last_file|N|link_orig|N|link|N|"TRIES: "|trys|N|"HTTPC: "|http_code|N|cont_type|FC;
	
	incResponseCode(http_code);
	goto get_next_link;
	
	return 0;
}

void downloadAll()
{
	mutex_main = CreateMutex(NULL, FALSE, NULL);
	mutex_status = CreateMutex(NULL, FALSE, NULL);
	mutex_print = CreateMutex(NULL, FALSE, NULL);
	mutex_fails = CreateMutex(NULL, FALSE, NULL);
	mutex_thread_num = CreateMutex(NULL, FALSE, NULL);
	
	lstatus.Reserve(~linkf);
	lstatus.Fill(0);
	
	// Initialise https library
	curl_global_init(CURL_GLOBAL_ALL);
	
	p|SCP(0, 0);
	
	for(ui64 i = 0; i < MAX_THREADS; ++i)
	{
		p|L("Thread ")|SPCN(0)|SPN(2)|i|':'|N;
	}
	
	for(ui64 i = 0; i < MAX_THREADS; ++i)
	{
		CreateThread(NULL, 0, downloader, (void *)i, 0, NULL);
	}
}

HANDLE swtm;
LARGE_INTEGER swtout;
bool64 sw_stop;

THREAD stopwatch(void *param)
{	
	swtout.QuadPart = -10000000; // 1 second
	
	SYSTEMTIME lt, slt;
	GetLocalTime(&lt);
	slt = lt;
	ui64 ssec = (ui64)(slt.wSecond + slt.wMinute * 60 + slt.wHour * 3600 + slt.wDay * 86400);
	
	ui64 pre_ms = lt.wMilliseconds;
	
	if(pre_ms > 20) // Wait for system clock to catch up and make delay ~ 20 ms
	{
		Sleep(1000 - (DWORD)pre_ms + 20);
	}
	else if(pre_ms < 15)
	{
		Sleep(15 - (DWORD)pre_ms);
	}
	
	while(1)
	{
		SetWaitableTimer(swtm, &swtout, 0, NULL, NULL, FALSE);
		
		GetLocalTime(&lt);
		ui64 sec = (ui64)(lt.wSecond + lt.wMinute * 60 + lt.wHour * 3600 + lt.wDay * 86400);
		ui64 spas = sec - ssec;
		
		ui64 d = spas/86400, h = spas%86400/3600, m = spas%86400%3600/60, s = spas%86400%3600%60;
		
		WaitForSingleObject(mutex_print, INFINITE); // CRITICAL SECTION START ==================
		p|SCP(0, MAX_THREADS+2)|SPC('0')|d|L(" d ")|SPN(2)|h|':'|SPN(2)|m|':'|SPN(2)|s|DP;
		ReleaseMutex(mutex_print); // CRITICAL SECTION END =====================================
		
		if(lt.wMilliseconds < pre_ms)
		{
			swtout.QuadPart -= 10000; // 1 ms
			if(lt.wMilliseconds < 15)
			{
				swtout.QuadPart -= 10000;
			}
		}
		else if(lt.wMilliseconds > pre_ms)
		{
			swtout.QuadPart += 10000; // 1 ms
			if(lt.wMilliseconds > 25)
			{
				swtout.QuadPart += 10000;
			}
		}
		
		pre_ms = lt.wMilliseconds;
		WaitForSingleObject(swtm, INFINITE);
		
		if(threads_active == 0)
		{
			return 0;
		}
	}		
	
	return 0;
}

void initiateDownload()
{
	p|DC|L("LNKSID: ");
	r > starting_link_idx;
	p|SCP(0, 0)|S(40);
	
	p|SCP(0, MAX_THREADS+1)|loadLinks()|L(" links loaded!")|N;
	downloadAll();
	
	swtm = CreateWaitableTimer(NULL, FALSE, NULL);
	CreateThread(NULL, 0, stopwatch, NULL, 0, NULL);
	
	p|P|P|P;
	all_threads_stop = true;
	p|DC|SCP(0, MAX_THREADS+3)|L("Aborting all threads!")|N;
	p|P|P|P|P|P;
	
	curl_global_cleanup();
}

void getFileExt(const wtxt &fn, wtxt &ext)
{
	ui64 dot = txtfe(fn, TEND, '.');
	
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
wtxt fext = MAX_PATH;
wtxt fn;
binf fhead = 32;

void restoreImgExt(const wtxt &dir)
{	
	HANDLE ffind;
	WIN32_FIND_DATAW fdata;
	ui64 idx = 0;
	
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
		p|PE|L(" In FindFirstFileExA!")|N;
		return;
	}

	do
	{
		txtsdt(fn, MAX_PATH, 0, fdata.cFileName); // Always set direct mianpulated text back to 0!
		txtszu(fn);
		
		if((~fn == 1 && fn[0] == '.') || (~fn == 2 && fn[0] == '.' && fn[1] == '.')) // Maybe not skip?
		{
			continue;
		}
		
		full_fn += dir, full_fn += '\\', full_fn += fn;
		getFileExt(full_fn, fext);
		
		if(fext != WL(".txt") && fext != WL(".jpg") && fext != WL(".jpeg") && fext != WL(".png") && fext != WL(".gif"))
		{
			filer fr = full_fn;
			fr > FRL(32) > fhead > FC;
			if(~fhead > 5		&& // Is a GIF
			fhead[0] == 'G'		&&
			fhead[1] == 'I'		&&
			fhead[2] == 'F'		&&
			fhead[3] == '8'		&&
			fhead[4] == '9'		&&
			fhead[5] == 'a')
			{
				wtxt nfn = full_fn;
				nfn += WL(".gif");
				MoveFile(full_fn, nfn);
			}
			else if(~fhead > 3	&& // Is an PNG
			fhead[0] == 0x89	&&
			fhead[1] == 'P'		&&
			fhead[2] == 'N'		&&
			fhead[3] == 'G')
			{
				wtxt nfn = full_fn;
				nfn += WL(".png");
				MoveFile(full_fn, nfn);
			}
			else if(~fhead > 2	&& // Is an JPG
			fhead[0] == 0xFF	&&
			fhead[1] == 0xD8	&&
			fhead[2] == 0xFF)
			{
				wtxt nfn = full_fn;
				nfn += WL(".jpg");
				MoveFile(full_fn, nfn);
			}
		}
		
		txtclr(full_fn);
		txtclr(fext);
		p|idx++|CR;
	}
	while(FindNextFileW(ffind, &fdata));
	FindClose(ffind);
	
	txtclr(fnd_all);
	txtsdt(fn, 0, 0, NULL); // Directly mianpulated text set back to 0 before it goes out of scope
}

wtxta indexes = 30000;

void removeNotImg(const wtxt &dir)
{	
	HANDLE ffind;
	WIN32_FIND_DATAW fdata;
	ui64 idx = 0;
	
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
		p|PE|L(" In FindFirstFileExA!")|N;
		return;
	}

	do
	{
		txtsdt(fn, MAX_PATH, 0, fdata.cFileName); // Always set direct mianpulated text back to 0!
		txtszu(fn);
		
		if((~fn == 1 && fn[0] == '.') || (~fn == 2 && fn[0] == '.' && fn[1] == '.')) // Maybe not skip?
		{
			continue;
		}
		
		full_fn += dir, full_fn += '\\', full_fn += fn;
		getFileExt(full_fn, fext);
		
		if(fext == WL(".not_an_image"))
		{
			indexes << txtsp(fn, 0, txtf(fn, 0, '.')-1);
			DeleteFileW(full_fn);
		}
		
		txtclr(full_fn);
		txtclr(fext);
		p|idx++|CR;
	}
	while(FindNextFileW(ffind, &fdata));
	FindClose(ffind);
	
	p|N;
	
	for(ui64 i = 0; i < ~indexes; ++i)
	{
		DeleteFileW(dir + '\\' + indexes[i] + WL(".txt"));
		p|i|CR;
	}
	
	txtclr(fnd_all);
	txtsdt(fn, 0, 0, NULL); // Directly mianpulated text set back to 0 before it goes out of scope
}

txt redir_failed = 4096;

void getAllRedirectFailLinks(const wtxt &dir)
{	
	HANDLE ffind;
	WIN32_FIND_DATAW fdata;
	ui64 idx = 0;
	
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
		p|PE|L(" In FindFirstFileExA!")|N;
		return;
	}

	do
	{
		txtsdt(fn, MAX_PATH, 0, fdata.cFileName); // Always set direct mianpulated text back to 0!
		txtszu(fn);
		
		if((~fn == 1 && fn[0] == '.') || (~fn == 2 && fn[0] == '.' && fn[1] == '.')) // Maybe not skip?
		{
			continue;
		}
		
		full_fn += dir, full_fn += '\\', full_fn += fn;
		getFileExt(full_fn, fext);
		
		if(fext == WL(".txt"))
		{
			filer fr = full_fn;
			txt tf;
			fr > tf > FC;
			ui64 numbeg = txtf(tf, 0, L("HTTPC: ")) + 7;
			ui64 numend = txtf(tf, numbeg, '\n') - 1;
			ui64 code = t2i(txtsp(tf, numbeg, numend));
			if(code >= 300 && code < 400)
			{
				ui64 fst_le = txtf(tf, 0, '\n'); // First line end
				redir_failed += txtsp(tf, 0, txtf(tf, fst_le+1, '\n'));
			}
		}
		
		txtclr(full_fn);
		txtclr(fext);
		p|idx++|CR;
	}
	while(FindNextFileW(ffind, &fdata));
	FindClose(ffind);
	
	filep fp = "FAILED_REDIRECTS.TXT";
	fp|redir_failed|FC;
	
	txtclr(fnd_all);
	txtsdt(fn, 0, 0, NULL); // Directly mianpulated text set back to 0 before it goes out of scope
}

txta succ_links = 30000;		// All unique successfull links
txta succ_links_nuniq = 30000;	// All successfull links

void getAllFailedLinks(const wtxt &dir0, const wtxt &dir1)
{	
	HANDLE ffind;
	WIN32_FIND_DATAW fdata;
	ui64 idx = 0;
	
	fnd_all += dir0, fnd_all += star;
	
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
		p|PE|L(" In FindFirstFileExA!")|N;
		return;
	}

	do
	{
		txtsdt(fn, MAX_PATH, 0, fdata.cFileName); // Always set direct mianpulated text back to 0!
		txtszu(fn);
		
		if((~fn == 1 && fn[0] == '.') || (~fn == 2 && fn[0] == '.' && fn[1] == '.')) // Maybe not skip?
		{
			continue;
		}
		
		full_fn += dir0, full_fn += '\\', full_fn += fn;
		getFileExt(full_fn, fext);
		
		if(fext == WL(".txt"))
		{
			filer fr = full_fn;
			txt tf;
			fr > tf > FC;
			ui64 fst_le = txtf(tf, 0, '\n'); // First line end
			txt fin_link = txtsp(tf, fst_le+1, txtf(tf, fst_le+1, '\n')-1);
			succ_links.AddUnique(fin_link);
			succ_links_nuniq << fin_link;
		}
		
		txtclr(full_fn);
		txtclr(fext);
		p|idx++|CR;
	}
	while(FindNextFileW(ffind, &fdata));
	FindClose(ffind);
	
	txtclr(fnd_all);
	txtsdt(fn, 0, 0, NULL); // Directly mianpulated text set back to 0 before it goes out of scope
	
	idx = 0;
	fnd_all += dir1, fnd_all += star;
	
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
		p|PE|L(" In FindFirstFileExA!")|N;
		return;
	}

	do
	{
		txtsdt(fn, MAX_PATH, 0, fdata.cFileName); // Always set direct mianpulated text back to 0!
		txtszu(fn);
		
		if((~fn == 1 && fn[0] == '.') || (~fn == 2 && fn[0] == '.' && fn[1] == '.')) // Maybe not skip?
		{
			continue;
		}
		
		full_fn += dir1, full_fn += '\\', full_fn += fn;
		getFileExt(full_fn, fext);
		
		if(fext == WL(".txt"))
		{
			filer fr = full_fn;
			txt tf;
			fr > tf > FC;
			ui64 fst_le = txtf(tf, 0, '\n'); // First line end
			txt fin_link = txtsp(tf, fst_le+1, txtf(tf, fst_le+1, '\n')-1);
			succ_links.AddUnique(fin_link);
			succ_links_nuniq << fin_link;
		}
		
		txtclr(full_fn);
		txtclr(fext);
		p|idx++|CR;
	}
	while(FindNextFileW(ffind, &fdata));
	FindClose(ffind);
	
	p|L("TOTAL SUCC LINKZ UNIQUE: ")|~succ_links|N;
	p|L("TOTAL SUCC LINKZ: ")|~succ_links_nuniq|N;
	
	loadLinks();
	lstatus.Reserve(~linkf);
	lstatus.Fill(0);
	
	for(ui64 i = 0; i < ~linkf; ++i)
	{
		if(i % 1024 == 0)
		{
			p|i|CR;
		}
		
		for(ui64 j = 0; j < ~succ_links; ++j)
		{
			if(linkf[i][0] == 'D')
			{
				continue;
			}
			
			if(linkf[i] == succ_links[j])
			{
				lstatus[i] = true;
				break;
			}
		}
	}
	
	filep tp = "D:\\vbox\\all_failed_links.txt";
	
	for(ui64 i = 0; i < ~linkf; ++i)
	{
		if(!lstatus[i])
		{
			tp|linkf[i]|N;
		}
	}
	
	txtclr(fnd_all);
	txtsdt(fn, 0, 0, NULL); // Directly mianpulated text set back to 0 before it goes out of scope
}

void getAllZeroSizeLinks(const wtxt &dir)
{	
	HANDLE ffind;
	WIN32_FIND_DATAW fdata;
	ui64 idx = 0;
	
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
		p|PE|L(" In FindFirstFileExA!")|N;
		return;
	}

	do
	{
		txtsdt(fn, MAX_PATH, 0, fdata.cFileName); // Always set direct mianpulated text back to 0!
		txtszu(fn);
		
		if((~fn == 1 && fn[0] == '.') || (~fn == 2 && fn[0] == '.' && fn[1] == '.')) // Maybe not skip?
		{
			continue;
		}
		
		full_fn += dir, full_fn += '\\', full_fn += fn;
		getFileExt(full_fn, fext);
		
		if(fext == WL(".txt"))
		{
			filer fr = full_fn;
			txt tf;
			fr > tf > FC;
			ui64 fst_le = txtf(tf, 0, '\n'); // First line end
			redir_failed += txtsp(tf, 0, txtf(tf, fst_le+1, '\n'));
		}
		
		txtclr(full_fn);
		txtclr(fext);
		p|idx++|CR;
	}
	while(FindNextFileW(ffind, &fdata));
	FindClose(ffind);
	
	filep fp = "FAILED_ZEROSIZE.TXT";
	fp|redir_failed|FC;
	
	txtclr(fnd_all);
	txtsdt(fn, 0, 0, NULL); // Directly mianpulated text set back to 0 before it goes out of scope
}

//D:\vbox\holywars.ru\comments\11521\index.html
//http://www.yaplakal.com/uploads/post-3-12296294826990.jpg
//http://s00.yaplakal.com/pics/pics_original/4/4/4/1451444.jpg
//TRIES: 3
//HTTPC: 200
//Content-Type: image/jpeg

//http:\\www.yaplakal.com\uploads\post-3-12296294826990.jpg

wtxt getImgFileName(const wtxt &id, const wtxt &idir)
{
	HANDLE ffind;
	WIN32_FIND_DATAW fdata;
	
	fnd_all = idir, fnd_all += star;
	
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
		p|PE|L(" In FindFirstFileExA!")|N;
		return wtxt();
	}

	do
	{
		txtsdt(fn, MAX_PATH, 0, fdata.cFileName); // Always set direct mianpulated text back to 0!
		txtszu(fn);
		
		if((~fn == 1 && fn[0] == '.') || (~fn == 2 && fn[0] == '.' && fn[1] == '.')) // Maybe not skip?
		{
			continue;
		}
		
		full_fn = idir, full_fn += '\\', full_fn += fn;
		getFileExt(full_fn, fext);
		wtxt pure_name = fn;
		txtd(pure_name, txtfe(pure_name, TEND, '.'), TEND);
		
		if(fext != WL(".txt") && pure_name == id)
		{
			FindClose(ffind);
			txtclr(fnd_all);
			txtsdt(fn, 0, 0, NULL); // Directly mianpulated text set back to 0 before it goes out of scope
			return full_fn;
		}
		
		txtclr(full_fn);
		txtclr(fext);
	}
	while(FindNextFileW(ffind, &fdata));
	FindClose(ffind);
	
	txtclr(fnd_all);
	txtsdt(fn, 0, 0, NULL); // Directly mianpulated text set back to 0 before it goes out of scope
	wtxt empty;
	return empty;
}

wtxt dir_base = MAX_PATH;
txt link_ext = 64;

void injectImgIntoMirror(const wtxt &bdir, const wtxt &idir, const txt &lnk, const wtxt &id)
{
	txt link_p = lnk;
	txt hex = 16;
	for(ui64 i = 0; i < ~link_p; ++i)
	{
		if(link_p[i] == '/')
		{
			link_p[i] = '\\';
		}
		
		if(link_p[i] == '%') // Uniform Resource Identifier (URI) to Internationalized Resource Identifier (IRI)
		{
			txtclr(hex);
			txtssz(hex, 2);
			hex[0] = link_p[i+1];
			hex[1] = link_p[i+2];
			txtr(link_p, i, 3, (char)h2i(hex));
		}
	}
	
	ui64 after_ext = txtfci(link_p, 0, L(".jpg"));
	if(after_ext == NFND)
	{
		after_ext = txtfci(link_p, 0, L(".png"));
		if(after_ext == NFND)
		{
			after_ext = txtfci(link_p, 0, L(".gif"));
			if(after_ext == NFND)
			{
				after_ext = txtfci(link_p, 0, L(".jpeg"));
				if(after_ext == NFND)
				{
					after_ext = txtfci(link_p, 0, L(".bmp"));
					if(after_ext == NFND)
					{
						p|"GAME OVER, MAN, GAME OVER! NO IMG EXTENTION FOUND!!"|P;
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
	
	if(after_ext != ~link_p)
	{
		txtd(link_p, after_ext, TEND);
	}
	
	// Recursive directory creation
	ui64 lslh = txtfe(link_p, TEND, '\\'); // Last slash
	ui64 pst = txtf(link_p, 0, L("\\\\"))+2; // Path start
	dir_base = bdir;
	dir_base += '\\';
	ui64 pre_slh_pos = pst-1;
	ui64 slh_pos = txtf(link_p, pst, '\\'); // Slash position
	while(slh_pos <= lslh)
	{
		dir_base += t2u16(txtsp(link_p, pre_slh_pos+1, slh_pos));
		CreateDirectoryW(dir_base, NULL);
		pre_slh_pos = slh_pos;
		slh_pos = txtf(link_p, slh_pos+1, '\\');
	}/*https://pp.userapi.com/c637624/v637624787/564a1/skJUOmQjqq4.jpg*/
	
	wtxt img_n = getImgFileName(id, idir);
	//wtxt ext = MAX_PATH;
	//getFileExt(img_n, ext);
	//ui64 orig_n_end = txtf(link_p, lslh+1, '.');
	////////////////////////////////GRAB LINK EXTENTION INSTEAD OF IMG FILE EXTENTION! IMPORTANT!
	
	// CORNER CASE: EXCEPTION_TXT_BUFF_OVERRUN
	// [TEXT]: [https:\\vignette.wikia.nocookie.net\ssb\images\6\60\Facepalm.png\revision\latest?cb=20141207005344]
	
	wtxt orig_name = lslh+1 >= ~link_p ? WL("") : t2u16(txtsp(link_p, lslh+1, TEND));
	wtxt new_path = dir_base + orig_name;
	
	CopyFile(img_n, new_path, FALSE);
}

void restoreMirrorLinks(const wtxt &bdir, const wtxt &idir)
{
	HANDLE ffind;
	WIN32_FIND_DATAW fdata;
	ui64 idx = 0;
	
	fnd_all += idir, fnd_all += star;
	
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
		p|PE|L(" In FindFirstFileExA!")|N;
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
		
		full_fn += idir, full_fn += '\\', full_fn += fn;
		getFileExt(full_fn, fext);
		
		if(fext == WL(".txt"))
		{
			filer fr = full_fn;
			txt tf;
			fr > tf > FC;
			ui64 fst_le = txtf(tf, 0, '\n'); // First line end
			txt fin_link = txtsp(tf, fst_le+1, txtf(tf, fst_le+1, '\n')-1);
			
			wtxt pure_name = fn;
			txtd(pure_name, txtfe(pure_name, TEND, '.'), TEND);
			
			/*wtxt ext = */injectImgIntoMirror(bdir, idir, fin_link, pure_name);
			//txt html_file = txtsp(tf, 0, fst_le-1);
			//txt html_src;
			//fr < FO(html_file) > html_src > FC;
			//
			//ui64 lnk_st = txtf(fin_link, 0, L("//"))+2; // Link start
			//ui64 lslh = txtfe(fin_link, TEND, '/'); // Last slash
			//ui64 dot = txtf(fin_link, lslh+1, '.');
			//
			//txt pure_link = txtsp(fin_link, lnk_st, dot-1);
			//ui64 html_dot = txtf(html_src, 0, pure_link) + ~pure_link;
			//ui64 html_ext_end = txtf(html_src, html_dot+1, '"') - 1;
			//txtr(html_src, html_dot, html_ext_end - html_dot + 1, wt2u8(ext));
			//
			//filep fp = html_file;
			//fp|TRN|html_src|FC;
		}
		
		txtclr(full_fn);
		txtclr(fext);
		
		if(++idx % 64 == 0)
		{
			p|idir|S|idx|CR;
		}
	}
	while(FindNextFileW(ffind, &fdata));
	FindClose(ffind);
	
	p|N;
	
	txtclr(fnd_all);
	txtsdt(fn, 0, 0, NULL); // Directly mianpulated text set back to 0 before it goes out of scope
}

void loadSuccLinks(const wtxt &succ_dir)
{
	HANDLE ffind;
	WIN32_FIND_DATAW fdata;
	ui64 idx = 0;
	
	fnd_all += succ_dir, fnd_all += star;
	
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
		p|PE|L(" In FindFirstFileExA!")|N;
		return;
	}

	do
	{
		txtsdt(fn, MAX_PATH, 0, fdata.cFileName); // Always set direct mianpulated text back to 0!
		txtszu(fn);
		
		if((~fn == 1 && fn[0] == '.') || (~fn == 2 && fn[0] == '.' && fn[1] == '.')) // Maybe not skip?
		{
			continue;
		}
		
		full_fn += succ_dir, full_fn += '\\', full_fn += fn;
		getFileExt(full_fn, fext);
		
		if(fext == WL(".txt"))
		{
			filer fr = full_fn;
			txt tf;
			fr > tf > FC;
			ui64 fst_le = txtf(tf, 0, '\n'); // First line end
			txt fin_link = txtsp(tf, fst_le+1, txtf(tf, fst_le+1, '\n')-1);
			succ_links.AddUnique(fin_link);
			//succ_links_nuniq << fin_link;
		}
		
		txtclr(full_fn);
		txtclr(fext);
		p|idx++|CR;
	}
	while(FindNextFileW(ffind, &fdata));
	FindClose(ffind);
	
	txtclr(fnd_all);
	txtsdt(fn, 0, 0, NULL); // Directly mianpulated text set back to 0 before it goes out of scope
}

ui64 loadPureLinks()
{
	linkf.Clear();
	filer fr = "D:\\hw\\all_ext_img_links.txt";
	//filer fr = L("D:\\vbox\\all_failed_links.txt");
	//filer fr = L("D:\\vbox\\all_miss_img_links.txt");
	//filer fr = L("D:\\P\\MT\\FAILED_REDIRECTS.TXT");
	if(!fr)
	{
		//fr < FO(L("all_failed_links.txt"));
		//fr < FO(L("all_miss_img_links.txt"));
		//fr < FO(L("FAILED_REDIRECTS.TXT"));
	}
	
	txt lnk;
	fr > lnk > FC;
	
	ui64 l_beg = 0;		// Line beginning
	txt file_link = 4096;	// File or link
	for(ui64 i = 0; i < ~lnk; ++i)
	{
		if(lnk[i] == '\n')
		{
			txtsp(file_link, lnk, l_beg, i-1); // Do not include newline
			linkf << file_link;
			
			if(i + 1 < ~lnk)
			{
				l_beg = i + 1;
			}
		}
	}
	
	lstatus.Reserve(~linkf);
	lstatus.Fill(0);
	
	loadSuccLinks(WL("D:\\vbox\\LNKD_PRELIM\\IMAGES_0_VPN"));
	loadSuccLinks(WL("D:\\vbox\\LNKD_PRELIM\\IMAGES_1_VPN_NOVPN"));
	loadSuccLinks(WL("D:\\vbox\\LNKD_PRELIM\\IMAGES_2_NOVPN"));
	loadSuccLinks(WL("D:\\vbox\\LNKD_PRELIM\\IMAGES_3_ZEROS"));
	
	for(ui64 i = 0; i < ~linkf; ++i)
	{
		if(i % 1024 == 0)
		{
			p|i|CR;
		}
		
		for(ui64 j = 0; j < ~succ_links; ++j)
		{
			if(linkf[i] == succ_links[j])
			{
				lstatus[i] = true;
				break;
			}
		}
	}
	
	filep tp = "D:\\hw\\all_failed_links.txt";
	tp|TRN;
	
	txta unique_failed = 40000;
	
	for(ui64 i = 0; i < ~linkf; ++i)
	{
		if(!lstatus[i])
		{
			unique_failed.AddUnique(linkf[i]);
		}
	}
	
	for(ui64 i = 0; i < ~unique_failed; ++i)
	{
		if(!lstatus[i])
		{
			tp|unique_failed[i]|N;
		}
	}
	
	return ~linkf;
}

i32 wmain()
{
	p|DC;
	//loadLinksWithExt();
	//analyseLinkTypes();
	//loadImageLinks();
	//loadLinksNotDown();
	//p|L("....")|P;
	//restoreImgExt(WL("D:\\vbox\\LNKD\\IMAGES")));
	//removeNotImg(WL("D:\\vbox\\LNKD\\IMAGES")));
	
	//restoreImgExt(WL("D:\\vbox\\LNKD_PRELIM\\IMAGES_4_NOVPN"));
	//p|P;
	//removeNotImg(WL("D:\\vbox\\LNKD_PRELIM\\IMAGES_4_NOVPN"));
	
	//getAllRedirectFailLinks(WL("D:\\vbox\\LNKD\\IMAGES")));
	//getAllFailedLinks(WL("D:\\vbox\\LNKD_PRELIM\\IMAGES_PRELIM", L"D:\\P\\MT\\IMAGES")));
	//getAllZeroSizeLinks(WL("D:\\vbox\\LNKD_PRELIM\\FAILED_0_SIZE")));
	
	//initiateDownload();
	
	//restoreMirrorLinks(WL("D:\\hw\\holywars.offline\\web"), WL("D:\\vbox\\LNKD_PRELIM\\IMAGES_3_ZEROS"));
	////p|L("P1DONE!")|P;
	//restoreMirrorLinks(WL("D:\\hw\\holywars.offline\\web"), WL("D:\\vbox\\LNKD_PRELIM\\IMAGES_2_NOVPN"));
	////p|L("P1DONE!")|P;
	//restoreMirrorLinks(WL("D:\\hw\\holywars.offline\\web"), WL("D:\\vbox\\LNKD_PRELIM\\IMAGES_1_VPN_NOVPN"));
	////p|L("P2DONE!")|P;
	//restoreMirrorLinks(WL("D:\\hw\\holywars.offline\\web"), WL("D:\\vbox\\LNKD_PRELIM\\IMAGES_0_VPN"));
	//restoreMirrorLinks(WL("D:\\hw\\holywars.offline\\web"), WL("D:\\vbox\\LNKD_PRELIM\\IMAGES_4_VPN"));
	//restoreMirrorLinks(WL("D:\\hw\\holywars.offline\\web"), WL("D:\\vbox\\LNKD_PRELIM\\IMAGES_4_NOVPN"));
	
	//loadPureLinks();
	
	p|TB|N|L("DONE!")|P;
	return 0;
}