/*
 * rcu.h
 *
 *  Created on: 2019-7-15
 *      Author: Brant
 */

#ifndef GUI_RCU_H_
#define GUI_RCU_H_

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct _ui_rcu_key_evt_data_
{
	unsigned int substitute		:8;
	unsigned int repeat_flag	:1;
	unsigned int reserved		:23;
} RcuKeyEvtData;


int send_rcu_key(unsigned int hk_code, BOOL repeat_flag);


#ifdef __cplusplus
}
#endif

#endif /* GUI_RCU_H_ */
