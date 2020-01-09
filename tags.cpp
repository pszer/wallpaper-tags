#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <array>

enum MODE { ERR , ADD , RM , CLEAR , DISP , QUERY , COUNT , UPDATE , PAPE };

const std::string INIT_MODE  = "init";
const std::string ADD_MODE   = "add";
const std::string RM_MODE    = "remove";
const std::string CLEAR_MODE = "clear";
const std::string DISP_MODE  = "display";
const std::string QUERY_MODE = "query";
const std::string UPDATE_MODE= "update";
const std::string COUNT_MODE = "count";
const std::string PAPE_MODE  = "pape";

const std::string INIT_MODE_USE  = "tags init\n"
                                   "creates a .tags file in working directory";
const std::string ADD_MODE_USE   = "tags add [file] {tags ...}\n"
                                   "add tags to file";
const std::string RM_MODE_USE    = "tags remove [file] {tags ...}\n"
                                   "remove specific tags from file";
const std::string CLEAR_MODE_USE = "tags clear {files ...}\n"
                                   "remove tagged files";
const std::string DISP_MODE_USE  = "tags display {files ...}\n"
                                   "print the tags of given files";
const std::string QUERY_MODE_USE = "tags query {'AND'} [tags ...]\n"
                                   "find all files with given tags.\n"
                                   "optional 'AND' flag queries for files with ALL given tags";
const std::string UPDATE_MODE_USE= "tags update\n"
                                   "removes entries for deleted files and sorts alphabetically tags";
const std::string COUNT_MODE_USE = "tags count {tags ...}\n"
                                   "counts occurences of given tags, if no tags given it counts all tags";
const std::string PAPE_MODE_USE  = "tags pape {'AND'} {tags ...}\n"
                                   "opens all images matching tags in feh, press enter to set 'pape\n"
                                   "optional 'AND' flag queries for files with ALL given tags";

const std::string HELP_MSG = "tags [option] ...\noptions: init/add/remove/clear/query/display/update/pape\n\n"
 + INIT_MODE_USE + "\n\n"
 + ADD_MODE_USE + "\n\n"
 + RM_MODE_USE + "\n\n"
 + CLEAR_MODE_USE + "\n\n"
 + DISP_MODE_USE + "\n\n"
 + QUERY_MODE_USE + "\n\n"
 + COUNT_MODE_USE + "\n\n"
 + UPDATE_MODE_USE + "\n\n"
 + PAPE_MODE_USE;

const std::string TAGS_FILE = ".tags";

struct File {
	std::string fname;
	std::vector<std::string> tags;
};

// 1 for success
// 0 for error
int read_files( void );
int write_files( void );

std::vector<File> files;
File * find_file_fname(std::string fname);
bool file_has_tag(File * file, const std::string& tag);
std::vector<File> query_tags(const std::vector<std::string>& tags);
std::vector<File> query_tags_intersect(const std::vector<std::string>& tags);

void print_file(File * file);
std::string format_tag(const std::string& tag);

std::stringstream op_output = std::stringstream("");
int add(int argc, char ** argv); 
int remove(int argc, char ** argv); 
int clear(int argc, char ** argv); 
int display(int argc, char ** argv); 
int query(int argc, char ** argv); 
int count(int argc, char ** argv); 
int update(int argc, char ** argv); 
int pape(int argc, char ** argv);

bool string_less_than(const std::string& a, const std::string& b);
void sort_string_vec(std::vector<std::string>& vec);
std::string make_lowercase_str(const std::string& str);
bool file_exists(const std::string& fname);

int main(int argc, char ** argv) {
	if (argc == 1) {
		std::cout << HELP_MSG << std::endl;
		return 0;
	}

	std::string mode = argv[1];

	if (mode == INIT_MODE) {
		std::ifstream in_f(TAGS_FILE);
		if (in_f) return 0;

		std::ofstream out_f(TAGS_FILE, std::ofstream::trunc);
		//out_f << '\n';

		out_f.close();
		return 0;
	} 

	if (!read_files()) {
		std::cerr << "error reading .tags" << std::endl;
		return -1;
	}

	MODE m = ERR;
	if (mode == ADD_MODE) { m = ADD; }
	else if (mode == RM_MODE) { m = RM; }
	else if (mode == CLEAR_MODE) { m = CLEAR; }
	else if (mode == DISP_MODE) { m = DISP; }
	else if (mode == QUERY_MODE) { m = QUERY; }
	else if (mode == COUNT_MODE) { m = COUNT; }
	else if (mode == UPDATE_MODE) { m = UPDATE; }
	else if (mode == PAPE_MODE) { m = PAPE; }

	if (m == ERR) {
		std::cerr << HELP_MSG << std::endl;
		return -1;
	}

	#define CASE(en,func) case en: func(argc,argv); break;
	switch (m) {
	CASE (ADD,   add);
	CASE (RM,    remove);
	CASE (CLEAR, clear);
	CASE (DISP,  display);
	CASE (QUERY, query);
	CASE (COUNT, count);
	CASE (UPDATE,update);
	CASE (PAPE,  pape);
	}

	return 0;
}

int read_files(void) {
	std::ifstream istr(TAGS_FILE);
	if (!istr) return 0;

	std::string str;
	while (!istr.eof()) {
		File tagged_file;

		std::getline(istr, str);
		if (str.empty()) continue;

		bool first = true;
		std::string temp = "";

		std::size_t i, last=0;
		for (i = last;; ++i) {
			char c = str[i];
			if (c == ' ' || !c) {
				std::size_t pos = last;
				std::size_t len = i-last;

				last = i+1;
				if (len != 0) {
					temp = str.substr(pos, len);
					if (first) {
						first = false;
						tagged_file.fname = temp;
					} else {
						tagged_file.tags.push_back(temp);
					}
				}
			}

			if (!c) break;
		}

		if (first) continue;
		files.push_back(tagged_file);
	}

	istr.close();
	return 1;
}

int write_files(void) {
	std::ofstream ostr(TAGS_FILE, std::ofstream::trunc);

	for (auto file : files) {
		ostr << file.fname;
		for (auto tag : file.tags) {
			ostr << " " << tag;
		}
		ostr << "\n";
	}

	ostr.close();
	return 1;
}

File * find_file_fname(std::string fname) {
	for (auto f = files.begin(); f != files.end(); ++f)
		if (f->fname == fname) return &(*f);
	return nullptr;
}

bool file_has_tag(File * file, const std::string& tag) {
	for (auto t = file->tags.begin(); t != file->tags.end(); ++t)
		if (*t == tag) return true;
	return false;
}

std::vector<File> query_tags(const std::vector<std::string>& tags) {
	std::vector<File> q;

	for (auto f = files.begin(); f != files.end(); ++f) {
		for (auto& tag : tags) {
			if (file_has_tag(&(*f), tag)) {
				q.push_back(*f);
				break;
			}
		}
	}

	return q;
}

std::vector<File> query_tags_intersect(const std::vector<std::string>& tags) {
	std::vector<File> rec_files;

	if (tags.empty()) return rec_files;
	// first pass to get initial recognised files
	std::string first_tag = tags.at(0);
	for (auto f = files.begin(); f != files.end(); ++f) {
		if (file_has_tag(&(*f), first_tag)) {
			rec_files.push_back(*f);
		}
	}

	for (auto tag = tags.begin()+1; tag != tags.end(); ++tag) {
		if (rec_files.empty()) break;

		for (auto f = rec_files.begin(); f != rec_files.end();) {
			if (file_has_tag(&(*f), *tag))
				++f;
			else
				rec_files.erase(f);
		}
	}

	return rec_files;
}

int add(int argc, char ** argv) {
	if (argc < 3) return 0;

	std::string fname = std::string(argv[2]);

	// check if file exists
	if (!file_exists(fname)) {
		std::cout << "file \"" << fname << "\" doesn't exist" << std::endl;
		return 0;
	}

	File * f = find_file_fname(fname);
	if (f == nullptr) {
		File new_file;
		new_file.fname = fname;
		files.push_back(new_file);
		f = find_file_fname(fname);
	}

	int i;
	for (i = 3; i < argc; ++i) {
		std::string tag = make_lowercase_str(std::string(argv[i]));

		if (file_has_tag(f, tag))
			continue;

		f->tags.push_back(tag);
	}

	sort_string_vec(f->tags);

	return write_files();
}

int remove(int argc, char ** argv) {
	if (argc < 3) return 0;

	std::string fname = argv[2];
	File * f = find_file_fname(fname);
	if (f == nullptr)
		return 0;

	int i;
	for (i = 3; i < argc; ++i) {
		std::string tag = make_lowercase_str(std::string(argv[i]));

		for (auto ftag = f->tags.begin(); ftag != f->tags.end();) {
			if (*ftag == tag)
				f->tags.erase(ftag);
			else
				++ftag;
		}
	}

	return write_files();
}

int clear(int argc, char ** argv) {
	if (argc < 3) return 0;

	int i;
	for (i = 2; i < argc; ++i) {
		std::string fname = std::string(argv[i]);

		for (auto f = files.begin(); f != files.end(); ++f) {
			if (f->fname == fname) {
				files.erase(f);
				break;
			}
		}
	}

	return write_files();
}

int display(int argc, char ** argv) {
	int i;
	for (i = 2; i < argc; ++i) {
		std::string str = argv[i];

		File * f = find_file_fname(str);
		if (!f)
			continue;

		print_file(f);
	}
	return 1;
}

std::string format_tag(const std::string& tag) {
	const std::string reset_color = "\u001b[0m";

	#define PAIR(c,s) std::make_pair<std::string,std::string>(c,s)
	std::array<std::pair<std::string, std::string>, 14> cols = {
		PAIR("white"  , "\u001b[37m"),
		PAIR("red"    , "\u001b[31m"),
		PAIR("green"  , "\u001b[32m"),
		PAIR("blue"   , "\u001b[34m"),

		PAIR("yellow" , "\u001b[33m"),
		PAIR("orange" , "\u001b[33m"),

		PAIR("magenta", "\u001b[35m"),
		PAIR("purple" , "\u001b[35m"),
		PAIR("pink"   , "\u001b[35m"),

		PAIR("cyan"     , "\u001b[36m"),
		PAIR("turquoise", "\u001b[36m"),

		PAIR("4k", "\u001b[32m\u001b[47m"),
		PAIR("hd", "\u001b[36m\u001b[47m"),
		PAIR("sd", "\u001b[30m\u001b[47m")
	};
	#undef PAIR

	std::string col = "";
	for (auto& p : cols) {
		if (p.first == tag) {
			col = p.second;
		}
	}

	return "\u001b[40m" + col + tag + reset_color;
}

void print_file(File * f) {
	std::cout << f->fname << " : ";
	for (auto& tag : f->tags) {
		std::cout << format_tag(tag) << " ";
	}
	std::cout << ";" << std::endl;
}

int query(int argc, char ** argv) {
	if (argc == 2) return 0;

	bool intersect = std::string(argv[2]) == "AND";

	std::vector<std::string> tags;
	std::vector<File> fs;

	if (intersect) {
		for (int i = 3; i < argc; ++i)
			tags.push_back(make_lowercase_str(std::string(argv[i])));
		fs = query_tags_intersect(tags);
	} else {
		for (int i = 2; i < argc; ++i)
			tags.push_back(make_lowercase_str(std::string(argv[i])));
		fs = query_tags(tags);
	}

	for (auto& f : fs) {
		print_file(&f);
	}

	return 0;
}

int count(int argc, char ** argv) {
	bool query_all = (argc==2);
	// each pair is tag name and count
	std::vector<std::pair<std::string, int>> tags;
	if (!query_all) {
		for (int i = 2; i < argc; ++i)
			tags.push_back(
				std::make_pair<std::string, int>(
					make_lowercase_str(std::string(argv[i])),
					0));

		for (auto& f : files) {
			for (auto& tag : f.tags) {
				for (auto& tag_counter : tags) {
					if (tag == tag_counter.first) {
						++tag_counter.second;
						break;
					}
				}
			}
		}
	} else {
		for (auto& f : files) {
			for (auto& tag : f.tags) {
				bool counter_exists = false;

				for (auto& tag_counter : tags) {
					if (tag == tag_counter.first) {
						++tag_counter.second;
						counter_exists = true;
						break;
					}
				}

				if (!counter_exists)
					tags.push_back(
						std::make_pair<std::string, int>(
						make_lowercase_str(std::string(tag)),
						1));
			}
		}
	}

	struct {
		bool operator()(const std::pair<std::string,int>& a, const std::pair<std::string,int>& b) {
			return a.second > b.second;
		}
	} tag_comp;
	std::sort(tags.begin(), tags.end(), tag_comp);

	for (auto& counter : tags) {
		std::cout << format_tag(counter.first) << " : "
			<< counter.second << " occurence(s)" << std::endl;
	}
	return 1;
} 

int update(int argc, char ** argv) {
	int i;
	for (auto f = files.begin(); f != files.end();) {
		std::string& fname = f->fname;
		if (!file_exists(fname)) {
			files.erase(f);
		} else {
			sort_string_vec(f->tags);
			++f;
		}
	}
	return write_files();
} 

int pape(int argc, char ** argv) {
	query(argc, argv);

	std::vector<std::string> fnames;
	bool intersect = false;

	if (argc == 2) {
		for (auto& f : files) {
			fnames.push_back(f.fname);
		}
	} else {
		std::vector<std::string> tags;
		std::vector<File> fs;
		bool intersect = std::string(argv[2]) == "AND";

		if (intersect) {
			for (int i = 3; i < argc; ++i)
				tags.push_back(make_lowercase_str(std::string(argv[i])));
			fs = query_tags_intersect(tags);
		} else {
			for (int i = 2; i < argc; ++i)
				tags.push_back(make_lowercase_str(std::string(argv[i])));
			fs = query_tags(tags);
		}

		for (auto& f : fs)
			fnames.push_back(f.fname);
	}

	if (fnames.empty()) {
		std::cout << "no matching wallpapers" << std::endl;
		return 1;
	}

	std::stringstream shell_str;
	shell_str << "sxiv -q -t -o ";
	for (auto& f : fnames) {
		shell_str << "\"" << f << "\" ";
	}
	shell_str << " > /tmp/tagswall";

	std::system(shell_str.str().c_str());

	std::stringstream sstrm;
	sstrm << std::ifstream("/tmp/tagswall").rdbuf(); 
	std::string selected = sstrm.str();
	if (selected.empty()) return 0;

	std::size_t first_newline = selected.find_first_of('\n');
	if (first_newline != std::string::npos)
		selected = selected.substr(0, first_newline);

	std::string set_wall = std::string("feh -q --bg-scale " + selected);
	std::cout << set_wall << std::endl;
	std::system(set_wall.c_str());

	return 0;
}

bool string_less_than(const std::string& a, const std::string& b) {
	auto A = a.begin();
	auto B = b.begin();
	auto Aend = a.end();
	auto Bend = b.end();

	while (1) {
		if (A == Aend && B != Bend) return true;
		if (A != Aend && B == Bend) return false;

		if      (*A < *B) return true;
		else if (*A > *B) return false;

		++A;
		++B;
	}
}

void sort_string_vec(std::vector<std::string>& vec) {
	struct {
		bool operator()(const std::string& a, const std::string& b) {
			return string_less_than(a,b);
		}
	} strless;

	std::sort(vec.begin(), vec.end(), strless);
}

std::string make_lowercase_str(const std::string& str) {
	std::string result = str;
	for (auto& c : result) {
		if (c >= 'A' && c <= 'Z')
			c = 'a' + (c - 'A');
	}
	return result;
}

bool file_exists(const std::string& fname) {
	std::ifstream f(fname);
	if (!f) return false;
	f.close();
	return true;
}
