#include "parser.cpp"

int main() {
	parser mftParser;

	if(mftParser.init() != 0) return -1;
	//TODO(do things with the parsed MFT data in here)

	if(mftParser.cleanup() != 0) return -1;
	return 0;
}
