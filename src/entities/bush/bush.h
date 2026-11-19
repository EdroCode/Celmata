/*! \file bush.h
 *  \brief Definitions for the bush entity.
 */

#include "types.h"

/** \struct Bush
 *  \brief Represents a single bush with a position and food amount.
 *
 *  Members:
 *   - `pos`   Position of the bush on the canvas.
 *   - `food`  Current food units available.
 */
typedef struct Bush {
    Position pos;   /**< Position of the bush */
    int food;       /**< Current food units */
} Bush;

/** \struct BushList
 *  \brief Dynamic array container for multiple bushes.
 *
 *  Members:
 *   - `bushes`        Pointer to an array of Bush structures.
 *   - `bush_count`   Number of active bushes.
 *   - `bush_capacity` Allocated capacity of the array.
 */
typedef struct BushList {
    Bush *bushes;           /**< Pointer to bush entries */
    int bush_count;        /**< Number of active bushes */
    int bush_capacity;     /**< Allocated capacity */
} BushList;

/** \brief Create a BushList with `size` initial bushes.
 *  \param size Desired initial number of bushes.
 *  \return Pointer to a newly allocated BushList or NULL on error.
 */
struct BushList *create_bushes(int size);

/** \brief Update all bushes (e.g., decay food).
 *  \param list Pointer to the BushList to update.
 */
void update_bushes(BushList *list);

/** \brief Render all bushes onto the canvas.
 *  \param list Pointer to the BushList to draw.
 */
void draw_bushes(BushList *list);

/** \brief Retrieve up to `max_out` available bushes.
 *  \param list Pointer to the BushList.
 *  \param pos Output array for positions.
 *  \param ids Output array for indices.
 *  \param max_out Maximum number of bushes to retrieve.
 *  \return Number of bushes placed in the output arrays.
 */
int bush_available(BushList *list, Position *pos, int *ids, int max_out);
