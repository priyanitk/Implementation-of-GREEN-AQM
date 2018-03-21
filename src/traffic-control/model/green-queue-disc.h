/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016 NITK Surathkal
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Priyanka Gupta <jain.vivek.anand@gmail.com>
 *          Sandeep Singh <hisandeepsingh@hotmail.com>
 *          Mohit P. Tahiliani <tahiliani@nitk.edu.in>
 */

#ifndef GREEN_QUEUE_DISC_H
#define GREEN_QUEUE_DISC_H

#include <queue>
#include "ns3/packet.h"
#include "ns3/queue-disc.h"
#include "ns3/nstime.h"
#include "ns3/boolean.h"
#include "ns3/data-rate.h"
#include "ns3/timer.h"
#include "ns3/event-id.h"
#include "ns3/random-variable-stream.h"

namespace ns3 {

class UniformRandomVariable;

class GreenQueueDisc : public QueueDisc
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * \brief BlueQueueDisc Constructor
   */
  GreenQueueDisc ();

  /**
   * \brief BlueQueueDisc Destructor
   */
  virtual ~GreenQueueDisc ();

 /**
   * \brief Get the current value of the queue in bytes or packets.
   *
   * \returns The queue size in bytes or packets.
   */
  uint32_t GetQueueSize (void);

   /**
   * \brief Dispose of the object
   */
  virtual void DoDispose (void);

  /**
   * \brief Set the limit of the queue in bytes or packets.
   *
   * \param lim The limit in bytes or packets.
   */
  void SetQueueLimit (uint32_t lim);
/**
   * \brief Set the operating mode of this queue.
   *
   * \param mode The operating mode of this queue.
   */
  void SetMode (Queue::QueueMode mode);
 /**
   * \brief Get the encapsulation mode of this queue.
   *
   * \returns The encapsulation mode of this queue.
   */
  Queue::QueueMode GetMode (void);
//Queue::QueueMode GetMode (void);
  /**
   * \brief Get GREEN statistics after running.
   *
   * \returns The drop statistics.
   */
  //Stats GetStats ();

  /**
   * Assign a fixed random variable stream number to the random variables
   * used by this model.  Return the number of streams (possibly zero) that
   * have been assigned.
   *
   * \param stream first stream index to use
   * \return the number of stream indices assigned by this model
   */
  int64_t AssignStreams (int64_t stream);

 /**
   * \brief Stats
   */
  typedef struct
  {
    uint32_t pktDrop;      //!< Early probability drops: proactive
    uint32_t pktDrop_count;        //!< Drops due to queue limit: reactive
  } Stats;

  /**
   * \brief Get GREEN statistics after running.
   *
   * \returns The drop statistics.
   */
  Stats GetStats ();

  

  /**
   * \brief Initialize the queue parameters.
   */
  virtual void InitializeParams (void);

  virtual bool DoEnqueue (Ptr<QueueDiscItem> item);
  virtual Ptr<QueueDiscItem> DoDequeue (void);
  virtual Ptr<const QueueDiscItem> DoPeek (void) const;
  virtual bool CheckConfig (void);

  /**
   *\Update the Utilisation of link if it is less 
   */
  virtual double UpdateCurrentUtil(void);

  /**
   * \brief Check if a packet needs to be dropped due to probability drop
   * \returns false for no drop, true for drop
   */
  virtual bool DropEarly (void);
   
  /**
   *\Upadte the queue drop after the interval(window_time) 
   */
 virtual void UpdatequeueDrop (void);

private:
  Queue::QueueMode m_mode;                   //!< Mode (bytes or packets)
  uint32_t m_queueLimit;                        //!< Queue limit in bytes / packets
  Stats m_stats;                                //!< GREEN statistics
  Ptr<UniformRandomVariable> m_uv;              //!< Rng stream



 // ** Variables supplied by user
  double m_Pmark;                               //!< Marking Probability
  uint32_t m_meanPktSize;                       //!< Average Packet Size
  Time m_windowTime;
  double m_bw;
  uint32_t rtt_pkt;
  uint32_t m_flows;
   double gama_fun;
   double c;
   double Curr_Util;
   Time m_lastUpdateTime;
   Time m_idleStartTime;
      
};

}; // namespace ns3

#endif // GREEN_QUEUE_DISC_H
