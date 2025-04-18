#include "queue.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
inline static int cmp(struct list_head *a, struct list_head *b)
{
    return strcmp(list_entry(a, element_t, list)->value,
                  list_entry(b, element_t, list)->value);
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
/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return;
    struct list_head *tmp, *l = head, *r = head;
    for (int t = 1, len = 2, size = q_size(head); t < size;
         t <<= 1, len <<= 1, l = r = head) {
        for (int k = 0, i = 0, j = 0; k < size; k += len, i = 0, j = 0) {
            for (j = 0; j < t; j++)
                r = r->next;
            while (i < t && j < len && j + k < size) {
                if (descend ^ (cmp(l->next, r->next) > 0)) {
                    tmp = r->next;
                    r->next = tmp->next;
                    tmp->next = l->next;
                    l->next = tmp;
                    j++;
                } else
                    i++;
                l = l->next;
            }
            for (; j < len; j++)
                r = r->next;
            l = r;
        }
    }
    for (l = head->next, l->prev = head; l != head; l = l->next)
        l->next->prev = l;
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
        while (cur->prev != head && cmp(cur->prev, cur) > 0) {
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
        while (cur->prev != head && cmp(cur->prev, cur) < 0) {
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
                      bool descend)
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
        if (descend ^ (cmp(qcur_0, qcur_1) > 0)) {
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
int q_merge(struct list_head *head, bool descend)
{
    if (!head)
        return 0;
    int chain_size = q_size(head);
    struct list_head *l, *r, *save;
    for (int t = 1, i = 0; t < chain_size; t <<= 1, i = 0) {
        l = head->next, r = head->next->next, save = head->next;
        for (; i < chain_size; l = r->next, r = r->next->next, i += (t << 1)) {
            if (t + i < chain_size)
                merge_two(l, r, save, descend);
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