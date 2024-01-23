#ifndef MISC_HPP
#define MISC_HPP

#include <sstream>
#include <string>
#include <vector>


/**
 * @bug Qt和 std::filesystem有冲突，会导致找不到链接器。
 * 所以我使用Qt提供的方法来替代原本的封装，这将导致外部调用
 * 也出现改变。
 * @todo 所以我通过让Qt只在内部产生影响，但是我不知道这会
 * 不会导致位置的错误。
 */
#define USE_QT_FILESYSTEM 1


using byte = unsigned char;

//----------------------------------------------------------------------------

/** Misc. Utilities. */
namespace MiscUtil
{
//----------------------------------------------------------------------------
#if USE_QT_FILESYSTEM

std::string OpenFile(std::string n, std::ifstream& f);

//----------------------------------------------------------------------------
#elif

/** Get directory of the executable from arguments to main().
        If this function is called in main(), the directory of the executable
        will be extracted from the path to the executable in argv[0] (if
        possible) and added to the data directories searched in
        MiscUtil::OpenFile(). */
void FindProgramDir(int argc, char* argv[]);

std::string OpenFile(std::string name, std::ifstream& f);

#endif
//----------------------------------------------------------------------------

/** Converts a word to an array of bytes. */
void WordToBytes(unsigned word, byte* out);

/** Converts an array of four bytes into a word. */
unsigned BytesToWord(const byte* bytes);

/** Returns the number of bytes need to hold the given number of
        bits. Equal to (bits + 7)/8.
     */
int NumBytesToHoldBits(int bits);

/** Prints a vector with a space between elements. */
template<typename TYPE>
std::string PrintVector(const std::vector<TYPE>& v);

}

/** Prints a vector with a space between elements. */
template<typename TYPE>
std::string MiscUtil::PrintVector(const std::vector<TYPE>& v)
{
    std::ostringstream is;
    for (std::size_t i = 0; i < v.size(); ++i)
    {
        if (i) is << ' ';
        is << v[i];
    }
    return is.str();
}

//----------------------------------------------------------------------------


#endif // MISC_HPP
