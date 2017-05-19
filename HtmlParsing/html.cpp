#ifdef htmltest
#include "htmlparsing.h"
#include "../TypeTable/typetable.h"
int main() {
	htmlHandle html;
	std::string Files;
	std::vector<std::string> depFiles;
	std::unordered_map<std::string, std::vector<CodeAnalysis::lineNum>> collapseDB;
	html.htmlCreator( Files,  depFiles, collapseDB);
 }
#endif // htmltest
