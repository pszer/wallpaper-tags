#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

enum MODE { ERR , ADD , RM , DISP , QUERY , PAPE };

const std::string INIT_MODE  = "init";
const std::string ADD_MODE   = "add";
const std::string RM_MODE    = "remove";
const std::string DISP_MODE  = "display";
const std::string QUERY_MODE = "query";
const std::string PAPE_MODE  = "pape";

const std::string ADD_MODE_USE  = "tags add [file] {tags ...}\n"
                                  "add tags to file";
const std::string RM_MODE_USE   = "tags remove [file] {tags ...}/*\n"
                                  "remove specific tags from file or all (*)";
const std::string DISP_MODE_USE = "tags display {files ...}\n"
                                  "print the tags of given files";
const std::string QUERY_MODE_USE= "tags query {tags ...}\n"
                                  "find all files with given tags";
const std::string PAPE_MODE_USE = "tags pape {tags ...}\n"
                                  "opens all images matching tags in feh, press enter to set 'pape";

const std::string HELP_MSG = "tags init / add / remove / display / pape";

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
void print_file(File * file);
std::vector<std::string> query_tags(const std::vector<std::string>& tags);

std::stringstream op_output = std::stringstream("");
int add(int argc, char ** argv); 
int remove(int argc, char ** argv); 
int display(int argc, char ** argv); 
int query(int argc, char ** argv); 
int pape(int argc, char ** argv); 

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
	if (mode == ADD_MODE) {
		m = ADD;
	} else if (mode == RM_MODE) {
		m = RM;
	} else if (mode == DISP_MODE) {
		m = DISP;
	} else if (mode == QUERY_MODE) {
		m = QUERY;
	} else if (mode == PAPE_MODE) {
		m = PAPE;
	}

	if (m == ERR) {
		std::cerr << HELP_MSG << std::endl;
		return -1;
	}

	#define CASE(en,func) case en: func(argc,argv); break;
	switch (m) {
	CASE (ADD,   add);
	CASE (RM,    remove);
	CASE (DISP,  display);
	CASE (QUERY, query);
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

std::vector<std::string> query_tags(const std::vector<std::string>& tags) {
	std::vector<std::string> q;

	for (auto f = files.begin(); f != files.end(); ++f) {
		for (auto tag : tags) {
			if (file_has_tag(&(*f), tag)) {
				q.push_back(f->fname);
				break;
			}
		}
	}

	return q;
}

int add(int argc, char ** argv) {
	if (argc < 3) return 0;

	std::string fname = std::string(argv[2]);
	File * f = find_file_fname(fname);
	if (f == nullptr) {
		File new_file;
		new_file.fname = fname;
		files.push_back(new_file);
		f = find_file_fname(fname);
	}

	int i;
	for (i = 3; i < argc; ++i) {
		std::string tag = std::string(argv[i]);

		if (file_has_tag(f, tag))
			continue;

		f->tags.push_back(tag);
	}

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
		std::string tag = std::string(argv[i]);

		if (tag == "*") {
			f->tags.clear();
			return 0;
		}

		for (auto ftag = f->tags.begin(); ftag != f->tags.end();) {
			if (*ftag == tag)
				f->tags.erase(ftag);
			else
				++ftag;
		}
	}

	return write_files();
}

int display(int argc, char ** argv) {
	int i;
	for (i = 2; i < argc; ++i) {
		std::string str = argv[i];

		File * f = find_file_fname(str);
		if (!f) continue;

		print_file(f);
	}
	return 1;
}

void print_file(File * f) {
	std::cout << f->fname << " : ";
	for (auto tag : f->tags) {
		std::cout << tag << " ";
	}
	std::cout << ";" << std::endl;
}

int query(int argc, char ** argv) {
	for (auto f = files.begin(); f != files.end(); ++f) {

		int i;
		for (i = 2; i < argc; ++i) {
			std::string tag = argv[i];
			if (file_has_tag(&(*f), tag)) {
				print_file(&(*f));
				break;
			}
		}
	}
	return 0;
}

int pape(int argc, char ** argv) {
	std::vector<std::string> tags;
	int i;
	for (i = 2; i < argc; ++i) tags.push_back(argv[i]);
	std::vector<std::string> q = query_tags(tags);

	std::stringstream shell_str;
	shell_str << "feh -A \"feh --bg-scale %F\" ";

	for (auto f : q) {
		shell_str << "\"" << f << "\" ";
	}

	std::system(shell_str.str().c_str());

	return 0;
}
