#ifndef TOOLS_H
#define TOOLS_H

#include "types.h"

/**
 * @brief Parses command-line arguments and returns a populated Param structure.
 *
 * Iterates over argv starting at index 2 (index 0 is the program name,
 * index 1 is the target path). Recognised flags: -path, -size, -deep,
 * -cut <depth> <length>, -ban <name...>, -search <name...>, -stats, -out,
 * -debug. Sets the appropriate error flags on invalid or duplicate input.
 *
 * @param argc  Number of command-line arguments.
 * @param argv  Array of command-line argument strings.
 * @return      A Param struct describing every requested option.
 */
Param  getParameter(int argc, char *argv[]);

/**
 * @brief Converts a byte count into a human-readable size string.
 *
 * The returned string uses the most appropriate unit (O, KO, MO, GO or TO)
 * and is formatted with two decimal places. The caller is responsible for
 * freeing the returned string.
 *
 * @param size  File or directory size in bytes.
 * @return      A newly-allocated string such as "1.50 MO".
 */
String getSizeStr(long long size);

/**
 * @brief Recursively computes the total size of a directory.
 *
 * Sums the sizes of all regular files contained directly in @p dir, then
 * recurses into each sub-directory to include their contents.
 *
 * @param dir  The Directory whose total size should be calculated.
 * @return     Total size in bytes as a double.
 */
double getDirectorySize(Directory dir);

/**
 * @brief Counts the depth of a file-system path.
 *
 * Depth is defined as the number of '/' separators found in the path.
 * A leading "./" prefix is skipped before counting.
 *
 * @param path  The path string to evaluate.
 * @return      The depth (number of directory levels) of the path.
 */
int    depthCount(String path);

/**
 * @brief Converts a string to an integer (custom atoi implementation).
 *
 * Handles an optional leading '-' sign and rejects non-digit characters.
 * Returns the sentinel value -32767 when the input is NULL, empty, contains
 * non-digit characters, or consists only of '-'.
 *
 * @param input  The string to convert.
 * @return       The converted integer, or -32767 on invalid input.
 */
int    _atoi(char *input);

/**
 * @brief Extracts the file extension from a filename.
 *
 * The extension is defined as the substring starting at the last '.' in
 * @p str (inclusive). If no dot is found, an empty string is returned.
 * The caller is responsible for freeing the returned string.
 *
 * @param str  The filename string.
 * @return     A newly-allocated string containing the extension (e.g. ".c"),
 *             or an empty string if there is no extension.
 */
String getExt(char* str);

/**
 * @brief Registers or increments an extension in the global extension stats.
 *
 * Searches the global g_extStats array for an existing entry matching
 * @p ext. If found, its count is incremented; otherwise a new entry is
 * appended and g_nbExt is increased. A NULL or empty @p ext is treated as ".".
 *
 * @param ext  The extension string (e.g. ".c"). May be NULL.
 */
void   addExt(char* ext);

/**
 * @brief Sorts the global extension stats array by count (descending).
 *
 * Uses a bubble-sort algorithm on the g_extStats array so that the
 * most-frequent extensions appear first.
 */
void   triABulleExt();

/**
 * @brief Inserts an element into the global top-10 largest-files list.
 *
 * Maintains a sorted array of the ten largest files seen so far. If
 * @p element is larger than the current tenth-place entry, it is inserted
 * at the correct position and the smallest previous entry is evicted.
 * String fields are duplicated before insertion.
 *
 * @param element  The file element to consider for the top-10 list.
 */
void   addToTop10(Element element);

/**
 * @brief Returns the number of visible characters in a UTF-8 string.
 *
 * Counts only the leading bytes of each UTF-8 multi-byte sequence,
 * skipping continuation bytes (0x80–0xBF), so the result reflects
 * the actual number of visible glyphs rather than raw byte count.
 *
 * @param s  The UTF-8 encoded string.
 * @return   Number of visible characters in the string.
 */
int    strlenVis(const char* s);

/**
 * @brief Checks whether @p str is a substring of @p src.
 *
 * Performs a linear scan of @p src looking for the first occurrence of
 * @p str as a contiguous sub-sequence of characters.
 *
 * @param src  The string to search within.
 * @param str  The substring to look for.
 * @return     1 if @p str is found inside @p src, 0 otherwise.
 */
int    strpartcmp(char* src, char* str);

/**
 * @brief Returns the number of decimal digits in a non-negative integer.
 *
 * Special-cases zero, which is considered to have one digit.
 *
 * @param nb  The integer whose digit count is requested. Must be >= 0.
 * @return    Number of decimal digits (e.g. 100 → 3, 0 → 1).
 */
int    nbDigit(int nb);

/**
 * @brief Converts an integer to its decimal string representation.
 *
 * Handles negative values by prepending a '-' character. The caller is
 * responsible for freeing the returned string.
 *
 * @param nb  The integer to convert.
 * @return    A newly-allocated null-terminated decimal string.
 */
String intToString(int nb);

#endif