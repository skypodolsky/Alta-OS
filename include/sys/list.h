#ifndef LIST_HEADER
#define LIST_HEADER

struct list_head {
	struct list_head *prev, *next;
} __attribute__((packed));

/* Initializes absolute head of list			*/
/* @head - address of the list_head	structure	*/

#define LIST_HEAD_INIT(head)		\
		(head)->next = NULL;		\
		(head)->prev = NULL;

/* Inserts a new item '_head_new' after the '_head_after'	*/
/* @_head_new - an address of the item to add				*/
/* @_head_after - an address of item former to _head_new	*/

#define LIST_INSERT(_head_new, _head_after)				\
		(_head_new)->next = (_head_after)->next;		\
		(_head_new)->prev = (_head_after);				\
		if ((_head_after)->next)						\
			(_head_after)->next->prev = (_head_new);	\
		(_head_after)->next = _head_new;			

/* Deletes list node. You should release memory yourself.	*/
/* @_head - an address of list head you wand to delete		*/

#define LIST_DELETE(_head)							\
		if ((_head)->prev)							\
			(_head)->prev->next = (_head)->next;	\
		if ((_head)->next)							\
			(_head)->next->prev = (_head)->prev;

/* Delete all elements of type 's_type' list beginning from _head_ptr	*/
/* @s_type - type of data structure										*/
/* @_head_ptr - an address of head from which you want to delete		*/

#define LIST_FREE(s_type, _head_ptr)						\
		do {												\
			struct list_head * _it = (_head_ptr);			\
			list_for_each(_it) {							\
				void *_ptr = GET_LIST_DATA(s_type, (_it));	\
				LIST_DELETE((_it));							\
				free((_ptr));								\
			}												\
		} while (0);

/* Initializes loop through the whole list			*/
/* @_it - an address of absolute head of the list	*/

#define list_for_each(_it)		\
		for ((_it) = (_it)->next; (_it) != NULL; (_it) = (_it)->next)

/* Initializes reverse loop through the whole list			*/
/* @_it - an address of absolute head of the list	*/

#define list_for_each_prev(_it)	\
		for ( ; (_it)->prev != NULL; (_it) = (_it)->prev)

/* Retrieves pointer to data structure	*/
/* @s_type - type of data structure		*/
/* @_it - an address of current head	*/

#define GET_LIST_DATA(s_type, _it)	\
		(s_type *)((char *)(_it) - (unsigned int)&((s_type *)0)->head);

#endif /* LIST_HEADER */
