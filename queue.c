#include "queue.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define unlikely(x) __builtin_expect(!!(x), 0)
#define likely(x) __builtin_expect(!!(x), 1)
typedef int (*list_cmp_func_t)(struct list_head *a, struct list_head *b);



inline static int cmp(struct list_head *a, struct list_head *b)
{
    return strcmp(list_entry(a, element_t, list)->value,
                  list_entry(b, element_t, list)->value);
}

inline static int q_descend_cmp(struct list_head *a, struct list_head *b)
{
    return strcmp(list_entry(a, element_t, list)->value,
                  list_entry(b, element_t, list)->value) < 0;
}

inline static int q_ascend_cmp(struct list_head *a, struct list_head *b)
{
    return strcmp(list_entry(a, element_t, list)->value,
                  list_entry(b, element_t, list)->value) > 0;
}

/* Create an empty queue */
struct list_head *q_new()
{
    element_t *p = (element_t *) malloc(sizeof(element_t));
    if (!p)
        return NULL;
    p->list.prev = p->list.next = &p->list;
    p->value = NULL;
    return &p->list;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (head) {
        struct list_head *h = head;
        do {
            element_t *e = list_entry(h, element_t, list);
            h = h->next;
            q_release_element(e);
        } while (h != head);
    }
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new_e = malloc(sizeof(element_t));
    if (!new_e)
        return false;
    int len = strlen(s);
    new_e->value = malloc(len + 1);
    if (!new_e->value) {
        free(new_e);
        return false;
    }
    strncpy(new_e->value, s, len);
    new_e->value[len] = 0;
    new_e->list.prev = head;
    new_e->list.next = head->next;
    head->next->prev = &new_e->list;
    head->next = &new_e->list;
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new_e = malloc(sizeof(element_t));
    if (!new_e)
        return false;
    int len = strlen(s);
    new_e->value = malloc(len + 1);
    if (!new_e->value) {
        free(new_e);
        return false;
    }
    strncpy(new_e->value, s, len);
    new_e->value[len] = 0;
    new_e->list.next = head;
    new_e->list.prev = head->prev;
    head->prev->next = &new_e->list;
    head->prev = &new_e->list;
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || head->next == head)
        return NULL;
    element_t *rm = list_entry(head->next, element_t, list);
    head->next->next->prev = head;
    head->next = head->next->next;
    if (sp && rm && rm->value) {
        memset(sp, 0, bufsize);
        int len = strlen(rm->value);
        if (len > bufsize - 1)
            len = bufsize - 1;
        strncpy(sp, rm->value, len);
        sp[bufsize - 1] = '\0';
    }
    return rm;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || head->prev == head)
        return NULL;
    element_t *rm = list_entry(head->prev, element_t, list);
    head->prev->prev->next = head;
    head->prev = head->prev->prev;
    if (sp && rm && rm->value) {
        memset(sp, 0, bufsize);
        int len = strlen(rm->value);
        if (len > bufsize)
            len = bufsize - 1;
        strncpy(sp, rm->value, len);
        sp[bufsize - 1] = '\0';
    }
    return rm;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (head) {
        int res = 0;
        struct list_head *h = head->next;
        while (h != head) {
            res++;
            h = h->next;
        }
        return res;
    } else
        return 0;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || head->next == head)
        return false;
    int half_size = q_size(head) / 2;
    struct list_head *del = head->next;
    for (int i = 0; i < half_size; i++)
        del = del->next;
    del->next->prev = del->prev;
    del->prev->next = del->next;
    element_t *e = list_entry(del, element_t, list);
    q_release_element(e);
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || head->next == head)
        return false;
    for (struct list_head *cur = head->next; cur != head; cur = cur->next) {
        bool dup = false;
        while (cur->next != head && cmp(cur, cur->next) == 0) {
            dup = true;
            element_t *e = list_entry(cur->next, element_t, list);
            cur->next = cur->next->next;
            cur->next->prev = cur;
            q_release_element(e);
        }
        if (dup) {
            cur = cur->prev;
            element_t *e = list_entry(cur->next, element_t, list);
            cur->next = cur->next->next;
            cur->next->prev = cur;
            q_release_element(e);
        }
    }
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *nxt, *cur = head->next;
    while (cur->next && cur != head && cur->next != head) {
        nxt = cur->next;
        cur->next = nxt->next;
        nxt->prev = cur->prev;
        cur->prev = nxt;
        nxt->next = cur;
        nxt->prev->next = nxt;
        cur->next->prev = cur;
        cur = cur->next;
    }
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (head) {
        struct list_head *cur = head;
        do {
            cur->next = (struct list_head *) ((intptr_t) cur->next ^
                                              (intptr_t) cur->prev);
            cur->prev = (struct list_head *) ((intptr_t) cur->next ^
                                              (intptr_t) cur->prev);
            cur->next = (struct list_head *) ((intptr_t) cur->next ^
                                              (intptr_t) cur->prev);
            cur = cur->prev;
        } while (cur != head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || k == 1)
        return;
    struct list_head *cur = head->next;
    while (cur != head) {
        int cnt = 1;
        struct list_head *prv = cur->prev;
        while (cnt < k) {
            cur = cur->next;
            if (cur == head)
                return;
            cnt++;
        }
        struct list_head *nxt = cur->next;
        prv->next->next = nxt;
        nxt->prev = prv->next;
        prv->next = cur;
        do {
            cur->next = cur->prev;
            cur->prev = prv;
            prv = cur;
            cur = cur->next;
        } while (cur->next != nxt);
        cur->prev = prv;
        cur = nxt;
    }
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}
/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    int res = 0;
    struct list_head *cur;
    for (cur = head->prev; cur != head; res++, cur = cur->prev) {
        while (cur->prev != head && q_ascend_cmp(cur->prev, cur)) {
            element_t *e = list_entry(cur->prev, element_t, list);
            cur->prev = cur->prev->prev;
            cur->prev->next = cur;
            q_release_element(e);
        }
    }
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return res;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    int res = 0;
    struct list_head *cur;
    for (cur = head->prev; cur != head; res++, cur = cur->prev) {
        while (cur->prev != head && q_descend_cmp(cur->prev, cur)) {
            element_t *e = list_entry(cur->prev, element_t, list);
            cur->prev = cur->prev->prev;
            cur->prev->next = cur;
            q_release_element(e);
        }
    }
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return res;
}

static void merge_two(struct list_head *l,
                      struct list_head *r,
                      struct list_head *des,
                      list_cmp_func_t qcmp)
{
    l = list_entry(l, queue_contex_t, chain)->q;
    r = list_entry(r, queue_contex_t, chain)->q;
    des = list_entry(des, queue_contex_t, chain)->q;
    struct list_head *qcur_0 = l->next, *qcur_1 = r->next;
    if (__glibc_likely(qcur_1 != r)) {
        qcur_1->prev = l->prev;
        qcur_1->prev->next = qcur_1;
        r->prev->next = l;
        l->prev = r->prev;
        r->next = r->prev = r;
    } else
        qcur_1 = l;
    if (__glibc_unlikely(des != l)) {
        des->next = l->next;
        des->prev = l->prev;
        l->next = l->prev = l;
    }
    r = qcur_1;
    while (qcur_1 != l && qcur_0 != r && qcur_0 != l) {
        if (qcmp(qcur_0, qcur_1)) {
            des = qcur_1->next;
            qcur_1->next->prev = qcur_1->prev;
            qcur_1->prev->next = qcur_1->next;
            qcur_1->prev = qcur_0->prev;
            qcur_1->prev->next = qcur_0->prev = qcur_1;
            qcur_1->next = qcur_0;
            qcur_1 = des;
        } else {
            qcur_0 = qcur_0->next;
        }
    }
}



/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
static int _q_merge(struct list_head *head, list_cmp_func_t qcmp)
{
    int chain_size = q_size(head);
    struct list_head *l, *r, *save;
    for (int t = 1, i = 0; t < chain_size; t <<= 1, i = 0) {
        l = head->next, r = head->next->next, save = head->next;
        for (; i < chain_size; l = r->next, r = r->next->next, i += (t << 1)) {
            if (t + i < chain_size)
                merge_two(l, r, save, qcmp);
            else {
                if (save != l) {
                    l = list_entry(l, queue_contex_t, chain)->q;
                    save = list_entry(save, queue_contex_t, chain)->q;
                    if (l->next != l) {
                        save->next = l->next;
                        save->prev = l->prev;
                        l->next = l->prev = l;
                    }
                }
            }
            save = save->next;
        }
    }

    // https://leetcode.com/problems/merge-k-sorted-lists/
    return q_size(list_entry(head->next, queue_contex_t, chain)->q);
}



int q_merge(struct list_head *head, bool descend)
{
    if (!head)
        return 0;
    if (descend)
        return _q_merge(head, q_descend_cmp);
    else
        return _q_merge(head, q_ascend_cmp);
}


/* Sort elements of queue in ascending/descending order */

static struct list_head *merge(struct list_head *a,
                               struct list_head *b,
                               list_cmp_func_t qcmp)
{
    struct list_head *head = NULL, **tail = &head;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (!qcmp(a, b)) {
            *tail = a;
            tail = &a->next;
            a = a->next;
            if (!a) {
                *tail = b;
                break;
            }
        } else {
            *tail = b;
            tail = &b->next;
            b = b->next;
            if (!b) {
                *tail = a;
                break;
            }
        }
    }
    return head;
}

static void merge_final(struct list_head *head,
                        struct list_head *a,
                        struct list_head *b,
                        list_cmp_func_t qcmp)
{
    struct list_head *tail = head;
    uint8_t count = 0;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (!qcmp(a, b)) {
            tail->next = a;
            a->prev = tail;
            tail = a;
            a = a->next;
            if (!a)
                break;
        } else {
            tail->next = b;
            b->prev = tail;
            tail = b;
            b = b->next;
            if (!b) {
                b = a;
                break;
            }
        }
    }

    /* Finish linking remainder of list b on to tail */
    tail->next = b;
    do {
        if (unlikely(!++count))
            qcmp(b, b);
        b->prev = tail;
        tail = b;
        b = b->next;
    } while (b);

    /* And the final links to make a circular doubly-linked list */
    tail->next = head;
    head->prev = tail;
}

void _q_sort(struct list_head *head, list_cmp_func_t qcmp)
{
    struct list_head *list = head->next, *pending = NULL;
    size_t count = 0; /* Count of pending */

    if (list == head->prev) /* Zero or one elements */
        return;

    /* Convert to a null-terminated singly-linked list. */
    head->prev->next = NULL;


    do {
        size_t bits;
        struct list_head **tail = &pending;

        /* Find the least-significant clear bit in count */
        for (bits = count; bits & 1; bits >>= 1)
            tail = &(*tail)->prev;
        /* Do the indicated merge */
        if (likely(bits)) {
            struct list_head *a = *tail, *b = a->prev;

            a = merge(b, a, qcmp);
            /* Install the merged result in place of the inputs */
            a->prev = b->prev;
            *tail = a;
        }

        /* Move one element from input list to pending */
        list->prev = pending;
        pending = list;
        list = list->next;
        pending->next = NULL;
        count++;
    } while (list);

    /* End of input; merge together all the pending lists. */
    list = pending;
    pending = pending->prev;
    for (;;) {
        struct list_head *next = pending->prev;

        if (!next)
            break;
        list = merge(pending, list, qcmp);
        pending = next;
    }
    /* The final merge, rebuilding prev links */
    merge_final(head, pending, list, qcmp);
}


void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return;
    if (descend)
        _q_sort(head, q_descend_cmp);
    else
        _q_sort(head, q_ascend_cmp);
}
