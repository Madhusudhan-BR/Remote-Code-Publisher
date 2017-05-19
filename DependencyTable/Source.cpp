#ifdef test
#include "dependencyAnalysis.h"
#pragma warning(disable:4503)
int main() {

	dependencyAnalysis d;
	CodeAnalysis::TypeAnal ta;
	using Files = std::vector<std::string>;
	Files file;
	d.depAnal(ta, file);

	return 0;
}
#endif // test


 // Source.cpp


