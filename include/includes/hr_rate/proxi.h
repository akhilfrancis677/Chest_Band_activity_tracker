/*
 * proxi.h
 *
 *  Created on: 03-May-2020
 *      Author: afrancis
 */

#ifndef PROXI_H_
#define PROXI_H_




  typedef enum
  {
    PROXI_Limited,
    PROXI_Full,
  } PROXI_InitMode_t;

  void PROXI_Init( PROXI_InitMode_t init_mode );
  void PROXI_AdvUpdate( void );

#endif /* PROXI_H_ */
