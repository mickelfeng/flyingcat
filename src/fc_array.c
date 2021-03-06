/*
 * Copyright (C) 2014 BullSoft
 * Copyright (C) 2014 Shang Yuanchun <idealities@gmail.com>
 *
 *
 * You may redistribute it and/or modify it under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 3 of the License, or (at your option)
 * any later version.
 *
 * FlyingCat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FlyingCat. If not, write to:
 * The Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor
 * Boston, MA 02110-1301, USA.
 */

#include "fc_core.h"

fc_array_t *fc_array_create(fc_pool_t *pool, size_t n, size_t size)
{
    fc_array_t *arr;

    arr = fc_palloc(pool, sizeof(fc_array_t));
    if (arr == NULL) {
        return NULL;
    }

    return (fc_array_init(arr, pool, n, size) == FC_OK ? arr : NULL);
}

void *fc_array_push(fc_array_t *arr)
{
    void  *new;
    size_t s;
    fc_pool_t *p;

    if (arr->used == arr->nalloc) {
        s = arr->nalloc * arr->size;
        p = arr->pool;

        if ((u_char *)arr->data + s == p->d.last
            && p->d.last + arr->size <= p->d.end) {

            // current pool is enough for one more
            p->d.last += arr->size;
            arr->nalloc++;

        } else {
            // if not, allocate 2 times of current size
            new = fc_palloc(p, s * 2);
            if (new == NULL) {
                return NULL;
            }

            memcpy(new, arr->data, s);
            arr->nalloc *= 2;
            arr->data = new;
        }
    }

    return (u_char *)arr->data + arr->used++ * arr->size;
}

void *fc_array_pop(fc_array_t *arr)
{
    arr->used--;

    return (u_char *)arr->data + arr->used * arr->size;
}

void *fc_array_get(fc_array_t *arr, size_t idx)
{
    if (idx >= arr->nalloc) {
        return NULL;
    }

    return (u_char *)arr->data + idx * arr->size;
}

void fc_array_close(fc_array_t *arr)
{
    fc_pool_t *p = arr->pool;

    if ((u_char *)arr->data + arr->nalloc * arr->size == p->d.last) {
        p->d.last -= arr->nalloc * arr->size;
    }

    if (p->d.last - sizeof(fc_array_t) == (u_char *)arr) {
        p->d.last = (u_char *)arr;
    }
}
