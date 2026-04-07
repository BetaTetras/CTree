#ifndef OUTPUT_H
#define OUTPUT_H

#include <time.h>
#include "types.h"

/**
 * @brief Prints formatted text in the specified RGB foreground colour.
 *
 * Wraps a standard printf-style call with ANSI escape codes that set the
 * terminal foreground colour to the given RGB values, then resets to the
 * default colour after printing.
 *
 * @param r       Red component   (0–255).
 * @param g       Green component (0–255).
 * @param b       Blue component  (0–255).
 * @param format  printf-style format string.
 * @param ...     Additional arguments for the format string.
 */
void printf_RGB(int r, int g, int b, const char* format, ...);

/**
 * @brief Prints formatted text with both foreground and background RGB colours.
 *
 * Sets the terminal foreground to (fr, fg, fb) and background to
 * (br, bg, bb) using ANSI 24-bit colour escape codes, then resets after
 * printing.
 *
 * @param fr      Foreground red   (0–255).
 * @param fg      Foreground green (0–255).
 * @param fb      Foreground blue  (0–255).
 * @param br      Background red   (0–255).
 * @param bg      Background green (0–255).
 * @param bb      Background blue  (0–255).
 * @param format  printf-style format string.
 * @param ...     Additional arguments for the format string.
 */
void printf_RGB_BG(int fr, int fg, int fb, int br, int bg, int bb, const char* format, ...);

/**
 * @brief Prints a UTF-8 string with a linear colour gradient from one RGB value to another.
 *
 * Each visible glyph in @p text is printed with an individually computed
 * foreground colour interpolated between (r1, g1, b1) and (r2, g2, b2).
 * An optional @p step value adds an extra per-character brightness offset.
 * The final RGB values reached are written back through the out_* pointers,
 * allowing chained gradient calls to produce seamless colour transitions.
 *
 * @param r1      Starting red   component (0–255).
 * @param g1      Starting green component (0–255).
 * @param b1      Starting blue  component (0–255).
 * @param r2      Ending red     component (0–255).
 * @param g2      Ending green   component (0–255).
 * @param b2      Ending blue    component (0–255).
 * @param text    The UTF-8 string to print.
 * @param step    Per-character brightness increment added to the gradient.
 * @param out_r   Output: red   component of the last printed character. May be NULL.
 * @param out_g   Output: green component of the last printed character. May be NULL.
 * @param out_b   Output: blue  component of the last printed character. May be NULL.
 */
void printf_wave_utf8(int r1, int g1, int b1, int r2, int g2, int b2, const char* text, int step, int* out_r, int* out_g, int* out_b);

/**
 * @brief Maps a file size in bytes to a representative RGB gradient colour.
 *
 * The colour transitions from green (small files) through yellow and orange
 * to red (large files), giving an immediate visual indicator of file weight.
 * Size categories: < 1 KO (green), < 1 MO (yellow), < 1 GO (orange),
 * >= 1 GO (red).
 *
 * @param size  File size in bytes.
 * @param r     Output: red   component (0–255).
 * @param g     Output: green component (0–255).
 * @param b     Output: blue  component (0–255).
 */
void sizeToGradientColor(long long size, int* r, int* g, int* b);

/**
 * @brief Prints the full statistics panel to stdout.
 *
 * Displays three decorated boxes:
 *  - Numeric summary (total directories, files, and cumulative size).
 *  - Top-5 extensions ranked by file count, with a proportional bar.
 *  - Top-10 heaviest files with their names, paths, and sizes.
 *
 * Reads from the global variables g_nbDIR, g_nbFILE, g_globalSize,
 * g_extStats / g_nbExt, and g_topFiles.
 */
void printf_Stat();

/**
 * @brief Prints debug information about the active parameters and execution time.
 *
 * Only produces output when p.debug is non-zero. Prints all boolean flags,
 * ban and search lists, error flags, and the elapsed time between @p start
 * and @p end clock ticks.
 *
 * @param p      The Param structure describing the current run configuration.
 * @param start  Clock tick recorded at program start.
 * @param end    Clock tick recorded at program end.
 */
void printf_debug(Param p,clock_t start,clock_t end);

/**
 * @brief Prints the search-results panel to stdout.
 *
 * Displays a decorated box listing every element found by the -search
 * option, showing each element's name, type (FILE or DIR), and path.
 * Reads from the global arrays g_searchedElements and g_searchedElementsIndex.
 */
void printf_searched();

/**
 * @brief Prints a string with a cycling rainbow colour effect.
 *
 * Each UTF-8 glyph in @p string is rendered in the next colour of the
 * rainbow spectrum, advancing by @p pas per character. @p sens controls
 * the direction of colour cycling (0 = forward, 1 = reverse). Colour
 * components are clamped to [0, @p max]. The @p brightness factor scales
 * all RGB components uniformly before printing.
 *
 * The caller is responsible for freeing the returned array.
 *
 * @param rgb         Pointer to a 3-element int array [R, G, B] with the
 *                    starting colour. Values are clamped to @p max in-place.
 * @param brightness  Multiplicative brightness factor applied to each component.
 * @param string      The UTF-8 string to print.
 * @param pas         Step size for colour advancement per character.
 * @param sens        Colour-cycle direction: 0 for forward, 1 for reverse.
 * @param max         Maximum value for each RGB component (e.g. 255).
 * @return            A newly-allocated 3-element int array with the final
 *                    [R, G, B] values after printing the entire string.
 */
int* print_rainbow(int* rgb, float brightness, const char* string, int pas, int sens, int max);

/**
 * @brief Prints a two-tone filled progress bar using Unicode block characters.
 *
 * Renders @p value filled (bright rainbow) blocks followed by
 * (_Size - @p value) empty (dimmed) blocks. The colour of the filled portion
 * continues the rainbow from yellow, while the empty portion uses a darkened
 * variant of the colour reached at the end of the filled section.
 *
 * @param _Size  Total number of blocks in the bar.
 * @param value  Number of blocks to render as "filled" (active portion).
 */
void printBarre(int _Size, int value);

#endif