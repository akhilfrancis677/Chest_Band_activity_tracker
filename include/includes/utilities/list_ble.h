/*
 * list_ble.h
 *
 *  Created on: 26-Apr-2020
 *      Author: afrancis
 */

#ifndef LIST_BLE_H_
#define LIST_BLE_H_

#ifdef __GNUC__
typedef struct __packed _tListNode {
#else
typedef __packed struct _tListNode {
#endif
  struct _tListNode * next;
  struct _tListNode * prev;
} tListNode;

void LST_init_head (tListNode * listHead);

uint8_t LST_is_empty (tListNode * listHead);

void LST_insert_head (tListNode * listHead, tListNode * node);

void LST_insert_tail (tListNode * listHead, tListNode * node);

void LST_remove_node (tListNode * node);

void LST_remove_head (tListNode * listHead, tListNode ** node );

void LST_remove_tail (tListNode * listHead, tListNode ** node );

void LST_insert_node_after (tListNode * node, tListNode * ref_node);

void LST_insert_node_before (tListNode * node, tListNode * ref_node);

int list_get_size (tListNode * listHead);

void list_get_next_node (tListNode * ref_node, tListNode ** node);

void list_get_prev_node (tListNode * ref_node, tListNode ** node);


#endif /* LIST_BLE_H_ */
