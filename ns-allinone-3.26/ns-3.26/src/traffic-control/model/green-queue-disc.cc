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
 * Authors: Priyanka Gupta<sourabhjain560@outlook.com>
 *          Mohit P. Tahiliani <tahiliani@nitk.edu.in>
 */

#include "ns3/log.h"
#include "ns3/enum.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/simulator.h"
#include "ns3/abort.h"
#include "green-queue-disc.h"
#include "ns3/drop-tail-queue.h"
//#include "ns3/ipv4-packet-disc-item.h"
//#include "ns3/ipv4-packet-filter-item.h"



namespace ns3 {

 //std::map<Ipv4Address,unsigned int> qlen_v;
 //std::map<Ipv4Addess, pckt_info> pckt_map;

 //Ipv4Header hdr;
 //Ipv4Header src;

NS_LOG_COMPONENT_DEFINE ("GreenQueueDisc");

NS_OBJECT_ENSURE_REGISTERED (GreenQueueDisc);

TypeId GreenQueueDisc::GetTypeId (void)
{

    static TypeId tid = TypeId ("ns3::GreenQueueDisc")
	.SetParent<QueueDisc> ()
	.SetGroupName ("TrafficControl")
	.AddConstructor<GreenQueueDisc> ()	
	.AddAttribute ("Mode",
                   "Determines unit for QueueLimit",
                   EnumValue (Queue::QUEUE_MODE_PACKETS),
                   MakeEnumAccessor (&GreenQueueDisc::SetMode),
                   MakeEnumChecker (Queue::QUEUE_MODE_BYTES, "QUEUE_MODE_BYTES",
                                    Queue::QUEUE_MODE_PACKETS, "QUEUE_MODE_PACKETS"))
	 .AddAttribute ("QueueLimit",
                   "Queue limit in bytes/packets",
                   UintegerValue (25),
                   MakeUintegerAccessor (&GreenQueueDisc::SetQueueLimit),
                   MakeUintegerChecker<uint32_t> ())
        .AddAttribute ("MeanPktSize",
                   "Average of packet size",
                   UintegerValue (1000),
                   MakeUintegerAccessor (&GreenQueueDisc::m_meanPktSize),
                   MakeUintegerChecker<uint32_t> ())
       
        .AddAttribute ("PMark",
                   "Marking Probabilty",
                   DoubleValue (0),
                   MakeDoubleAccessor (&GreenQueueDisc::m_Pmark),
                   MakeDoubleChecker<double> ())
         .AddAttribute ("WindowTime",
                   "Time interval during which Pmark cannot be updated,Curr_Util and flows can be updated",
                   TimeValue (Seconds (1.0)),
                   MakeTimeAccessor (&GreenQueueDisc::m_windowTime),
                   MakeTimeChecker ())
        .AddAttribute("Bandwidth",
                        "Bottleneck Bandwidth of Link",
                         DoubleValue(10),
                        MakeDoubleAccessor(&GreenQueueDisc::m_bw),
                        MakeDoubleChecker<double>())
 ;
  return tid;
}


GreenQueueDisc::GreenQueueDisc () :
  QueueDisc ()
{
  NS_LOG_FUNCTION (this);
  m_uv = CreateObject<UniformRandomVariable> ();
}

GreenQueueDisc::~GreenQueueDisc ()
{
  NS_LOG_FUNCTION (this);
}

void
GreenQueueDisc::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_uv = 0;
  QueueDisc::DoDispose ();
}

void
GreenQueueDisc::SetMode (Queue::QueueMode mode)
{
  NS_LOG_FUNCTION (this << mode);
  m_mode = mode;
}

Queue::QueueMode
GreenQueueDisc::GetMode (void)
{
  NS_LOG_FUNCTION (this);
  return m_mode;
}

void
GreenQueueDisc::SetQueueLimit (uint32_t lim)
{
  NS_LOG_FUNCTION (this << lim);
  m_queueLimit = lim;
}

uint32_t
GreenQueueDisc::GetQueueSize (void)
{
  NS_LOG_FUNCTION (this);
  if (GetMode () == Queue::QUEUE_MODE_BYTES)
    {
      return GetInternalQueue (0)->GetNBytes ();
    }
  else if (GetMode () == Queue::QUEUE_MODE_PACKETS)
    {
      return GetInternalQueue (0)->GetNPackets ();
    }
  else
    {
      NS_ABORT_MSG ("Unknown Green mode.");
    }
}

GreenQueueDisc::Stats
GreenQueueDisc::GetStats ()
{
  NS_LOG_FUNCTION (this);
  return m_stats;
}

int64_t
GreenQueueDisc::AssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (this << stream);
  m_uv->SetStream (stream);
  return 1;
}




void
GreenQueueDisc::InitializeParams (void)
{
  m_lastUpdateTime = Time (Seconds (0.0));
   m_idleStartTime = Time (Seconds (0.0));
   c=1.31;
   gama_fun=1.0;
   rtt_pkt=75.0;
  m_stats.pktDrop=0;
  m_stats.pktDrop_count=0;
   Curr_Util=0;
   m_flows=1;

}



bool
GreenQueueDisc::DoEnqueue (Ptr<QueueDiscItem> item)
{
  NS_LOG_FUNCTION (this << item);
  uint32_t nQueued = GetQueueSize ();


  double rand1=m_flows*m_meanPktSize*c;

  double rand2=m_bw*rtt_pkt*gama_fun;

  double rand_final=rand1/rand2;

  double m_Pmark=std::pow(rand_final,2);
  

 if ((GetMode () == Queue::QUEUE_MODE_PACKETS && nQueued >= m_queueLimit)
        || (GetMode () == Queue::QUEUE_MODE_BYTES && nQueued + item->GetPacketSize () > m_queueLimit))
      {
        
        // Drops due to queue limit: reactive
        m_stats.pktDrop++;
 
       Drop (item);
       return false;
      }
else
 {
   // Drop due to queue limit: reactive
    if(m_Pmark == 1.0)
    {
      m_stats.pktDrop++;
     UpdatequeueDrop();
      Drop (item);
      return false;
        std::cout<<"Packet Dropped"<<std::endl;
    }
    else
    {
      if(DropEarly ())
      {
        m_stats.pktDrop_count++;
         UpdatequeueDrop();
        std::cout<<"Packet Dropped"<<std::endl;
        Drop (item);
        return false;
      }
         std::cout<<"Packet Enqueued"<<std::endl;
      // No drop
      bool isEnqueued = GetInternalQueue (0)->Enqueue (item);
    /*  if(isEnqueued && nQueued<=lim)
        {
               hdr=item->GetHeader();
                src=hdr.GetSourceAddress();
                
                qlen_v[src]+=1;
                m_flows=qlen_v[src].size();
        }
   */
   
      NS_LOG_LOGIC ("\t bytesInQueue  " << GetInternalQueue (0)->GetNBytes ());
      NS_LOG_LOGIC ("\t packetsInQueue  " << GetInternalQueue (0)->GetNPackets ());

      return isEnqueued;
    }
 }

}



bool GreenQueueDisc::DropEarly (void)
{
  NS_LOG_FUNCTION (this);
  double u =  m_uv->GetValue ();
  if (u <= m_Pmark)
    {
      return true;
    }
  return false;
}  

//Update CurrentUtil
double GreenQueueDisc::UpdateCurrentUtil(void)
{       
    Curr_Util=(m_stats.pktDrop_count/m_bw);
      return Curr_Util;  
}

//Update QueueDrops
void GreenQueueDisc::UpdatequeueDrop (void)
{
   NS_LOG_FUNCTION (this);
   double util;
  Time now = Simulator::Now ();
  if (now - m_lastUpdateTime >= m_windowTime)
    {
        m_lastUpdateTime=now;        
        m_flows++;
        if(m_stats.pktDrop_count>0)
        {
          gama_fun=gama_fun*0.95;
        }
        else
        {
              util=UpdateCurrentUtil();
                if(util<0.98)
                {
                  gama_fun=((1+util)*gama_fun)/(2*util);
                }
        }
          
    }

}



Ptr<QueueDiscItem>
GreenQueueDisc::DoDequeue (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<QueueDiscItem> item = StaticCast<QueueDiscItem> (GetInternalQueue (0)->Dequeue ());

  NS_LOG_LOGIC ("Popped " << item);

  NS_LOG_LOGIC ("Number packets " << GetInternalQueue (0)->GetNPackets ());
  NS_LOG_LOGIC ("Number bytes " << GetInternalQueue (0)->GetNBytes ());

  if (GetInternalQueue (0)->IsEmpty ())
    {
      NS_LOG_LOGIC ("Queue empty");

      m_idleStartTime = Simulator::Now ();
      // Decrement the Pmark
      //m_isIdle = true;
      //DecrementPmark ();
    }

  return item;
}

Ptr<const QueueDiscItem>
GreenQueueDisc::DoPeek () const
{
  NS_LOG_FUNCTION (this);
  if (GetInternalQueue (0)->IsEmpty ())
    {
      NS_LOG_LOGIC ("Queue empty");
      return 0;
    }

  Ptr<const QueueDiscItem> item = StaticCast<const QueueDiscItem> (GetInternalQueue (0)->Peek ());

  NS_LOG_LOGIC ("Number packets " << GetInternalQueue (0)->GetNPackets ());
  NS_LOG_LOGIC ("Number bytes " << GetInternalQueue (0)->GetNBytes ());

  return item;
}

bool
GreenQueueDisc::CheckConfig (void)
{
  NS_LOG_FUNCTION (this);
  if (GetNQueueDiscClasses () > 0)
    {
      NS_LOG_ERROR ("GreenQueueDisc cannot have classes");
      return false;
    }

  if (GetNPacketFilters () > 0)
    {
      NS_LOG_ERROR ("GreenQueueDisc cannot have packet filters");
      return false;
    }

  if (GetNInternalQueues () == 0)
    {
      // create a DropTail queue
      Ptr<Queue> queue = CreateObjectWithAttributes<DropTailQueue> ("Mode", EnumValue (m_mode));
      if (m_mode == Queue::QUEUE_MODE_PACKETS)
        {
          queue->SetMaxPackets (m_queueLimit);
        }
      else
        {
          queue->SetMaxBytes (m_queueLimit);
        }
      AddInternalQueue (queue);
    }

  if (GetNInternalQueues () != 1)
    {
      NS_LOG_ERROR ("GreenQueueDisc needs 1 internal queue");
      return false;
    }

  if (GetInternalQueue (0)->GetMode () != m_mode)
    {
      NS_LOG_ERROR ("The mode of the provided queue does not match the mode set on the GreenQueueDisc");
      return false;
    }

  if ((m_mode ==  Queue::QUEUE_MODE_PACKETS && GetInternalQueue (0)->GetMaxPackets () < m_queueLimit)
      || (m_mode ==  Queue::QUEUE_MODE_BYTES && GetInternalQueue (0)->GetMaxBytes () < m_queueLimit))
    {
      NS_LOG_ERROR ("The size of the internal queue is less than the queue disc limit");
      return false;
    }

  return true;
}
}

//namespace ns3
