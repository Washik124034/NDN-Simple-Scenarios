/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2011-2015  Regents of the University of California.
 *
 * This file is part of ndnSIM. See AUTHORS for complete list of ndnSIM authors and
 * contributors.
 *
 * ndnSIM is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ndnSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ndnSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

// ndn-simple.cpp

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"

namespace ns3 {



int
main(int argc, char* argv[])
{
  // setting default parameters for PointToPoint links and channels
  Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("1Mbps"));
  Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("10ms"));
  Config::SetDefault("ns3::QueueBase::MaxSize", StringValue("20p"));

  // Read optional command-line parameters (e.g., enable visualizer with ./waf --run=<> --visualize
  CommandLine cmd;
  cmd.Parse(argc, argv);

  // Creating nodes
  NodeContainer nodes;
  nodes.Create(3);

  // Connecting nodes using two links
  PointToPointHelper p2p;
  p2p.Install(nodes.Get(0), nodes.Get(1));
  p2p.Install(nodes.Get(0), nodes.Get(2));
  p2p.Install(nodes.Get(1), nodes.Get(2));

  // Install NDN stack on all nodes
  ndn::StackHelper ndnHelper;
  ndnHelper.SetDefaultRoutes(true);
  ndnHelper.InstallAll();

  // Choosing forwarding strategy
  ndn::StrategyChoiceHelper::InstallAll("/prefix", "/localhost/nfd/strategy/multicast");

  // Installing applications

  // node 0 = Alice
  // Alice sending multicast Interests
  ndn::AppHelper AliceConsumerHelper("ns3::ndn::ConsumerCbr");
  AliceConsumerHelper.SetPrefix("/prefix/alice/newblock");
  AliceConsumerHelper.SetAttribute("Frequency", StringValue("1"));
  auto AliceApps = AliceConsumerHelper.Install(nodes.Get(0));

  // Alice receiving multicast Interest from Bob
  ndn::AppHelper AliceProducerBobHelper("ns3::ndn::Producer");
  AliceProducerBobHelper.SetPrefix("/prefix/bob/newblock/");
  AliceProducerBobHelper.SetAttribute("PayloadSize", StringValue("1024"));
  AliceApps.Add(AliceProducerBobHelper.Install(nodes.Get(0)));

  // Alice receiving multicast Interest from Charles
  ndn::AppHelper AliceProducerCharlesHelper("ns3::ndn::Producer");
  AliceProducerCharlesHelper.SetPrefix("/prefix/charles/newblock/");
  AliceProducerCharlesHelper.SetAttribute("PayloadSize", StringValue("1024"));
  AliceApps.Add(AliceProducerCharlesHelper.Install(nodes.Get(0)));
  AliceApps.Stop(Seconds(2.0));

  // node 1 = Bob
  // Bob sending multicast Interests
  ndn::AppHelper BobConsumerHelper("ns3::ndn::ConsumerCbr");
  BobConsumerHelper.SetPrefix("/prefix/bob/newblock");
  BobConsumerHelper.SetAttribute("Frequency", StringValue("1"));
  auto BobApps = BobConsumerHelper.Install(nodes.Get(1));

  // Bob receiving multicast Interest from Alice
  ndn::AppHelper BobProducerAliceHelper("ns3::ndn::Producer");
  BobProducerAliceHelper.SetPrefix("/prefix/alice/newblock");
  BobProducerAliceHelper.SetAttribute("PayloadSize", StringValue("1024"));
  BobApps.Add(BobProducerAliceHelper.Install(nodes.Get(1)));

  // Bob receiving multicast Interest from Charles
  ndn::AppHelper BobProducerCharlesHelper("ns3::ndn::Producer");
  BobProducerCharlesHelper.SetPrefix("/prefix/charles/newblock");
  BobProducerCharlesHelper.SetAttribute("PayloadSize", StringValue("1024"));
  BobApps.Add(BobProducerCharlesHelper.Install(nodes.Get(1)));
  BobApps.Stop(Seconds(2.0));

  // node 2 = Charles
  // Charles sending multicast Interests
  ndn::AppHelper CharlesConsumerHelper("ns3::ndn::ConsumerCbr");
  CharlesConsumerHelper.SetPrefix("/prefix/charles/newblock");
  CharlesConsumerHelper.SetAttribute("Frequency", StringValue("1"));
  auto CharlesApps = CharlesConsumerHelper.Install(nodes.Get(2));

  // Charles receiving multicast Interest from Alice
  ndn::AppHelper CharlesProducerAliceHelper("ns3::ndn::Producer");
  CharlesProducerAliceHelper.SetPrefix("/prefix/alice/newblock");
  CharlesProducerAliceHelper.SetAttribute("PayloadSize", StringValue("1024"));
  CharlesApps.Add(CharlesProducerAliceHelper.Install(nodes.Get(2)));

  // Charles receiving multicast Interest from Bob
  ndn::AppHelper CharlesProducerBobHelper("ns3::ndn::Producer");
  CharlesProducerBobHelper.SetPrefix("/prefix/bob/newblock");
  CharlesProducerBobHelper.SetAttribute("PayloadSize", StringValue("1024"));
  CharlesApps.Add(CharlesProducerBobHelper.Install(nodes.Get(2)));
  CharlesApps.Stop(Seconds(2.0));

  Simulator::Stop(Seconds(2.0));

  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}
