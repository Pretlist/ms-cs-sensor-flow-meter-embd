/**
  @brief: Test sensor flow meter 
  @file_name: test.cpp
  @author: skumar
  @Copyright (C) 2022 Pretlist - All Rights Reserved
 */
#include "common.hpp"
#include "gtest/gtest.h"

constexpr float gSensorData = 5.0F;

class SensorTest : public testing::Test {
 public:
  IMessagingServiceAdapterMock m_mockpubsub;
  static ms_nsq_client::IMessagingServiceAdapter::subscriberOptions m_subscribe_message;
  ms_cs_sensor_flow_meter_embd::SensorFlowMeter sensor_obj;

  SensorTest() : sensor_obj(&m_mockpubsub) {
    sensor_obj.Scan();
  }

  ~SensorTest() override = default;

  void SetUp() override {
  }

  void TearDown() override {
    EXPECT_CALL(m_mockpubsub, UnsubscribeAll()).WillOnce(::testing::Return(true));
    EXPECT_CALL(m_mockpubsub, StopPublisher()).Times(1);
  }

  static void NsqClientData(ms_nsq_client::IMessagingServiceAdapter::subscriberOptions* subOptions) {
    m_subscribe_message.ip = subOptions->ip;
    m_subscribe_message.connectionOption = subOptions->connectionOption;
    m_subscribe_message.connectionProtocol = subOptions->connectionProtocol;
    m_subscribe_message.topic = subOptions->topic;
    m_subscribe_message.channel = subOptions->channel;
    m_subscribe_message.callbackContext = subOptions->callbackContext;
    m_subscribe_message.callback = subOptions->callback;
  }

  static void VerifyFlowMeterInlet(const std::string& topic, folly::fbvector<unsigned char>* serialized_pkt) {
    FlowMeter::FlowMeterMsg data_published;
    data_published.ParseFromArray(serialized_pkt->data(), serialized_pkt->size());
    EXPECT_EQ(data_published.id(), Common::SENSOR_FLOW_METER_INLET);
    EXPECT_EQ(data_published.notification(), Common::NOTIFICATION_PERIODIC);
    EXPECT_EQ(data_published.status(), Common::SENSOR_STATUS_GOOD);
    EXPECT_EQ(data_published.flow(), gSensorData);
  }
  
  static void VerifyFlowMeterOutlet(const std::string& topic, folly::fbvector<unsigned char>* serialized_pkt) {
    FlowMeter::FlowMeterMsg data_published;
    data_published.ParseFromArray(serialized_pkt->data(), serialized_pkt->size());
    EXPECT_EQ(data_published.id(), Common::SENSOR_FLOW_METER_OUTLET);
    EXPECT_EQ(data_published.notification(), Common::NOTIFICATION_PERIODIC);
    EXPECT_EQ(data_published.status(), Common::SENSOR_STATUS_GOOD);
    EXPECT_EQ(data_published.flow(), gSensorData);
  }
};

// initilize static variables
ms_nsq_client::IMessagingServiceAdapter::subscriberOptions SensorTest::m_subscribe_message = {};

TEST_F(SensorTest, registerApp) {
  // Mock the init-nsq function
  EXPECT_CALL(m_mockpubsub, CreatePublisher(FLAGS_nsq_ip_port, ms_nsq_client::IMessagingServiceAdapter::E_TCP)).WillOnce(testing::Return(true));
  EXPECT_CALL(m_mockpubsub, Subscribe(::testing::Matcher<ms_nsq_client::IMessagingServiceAdapter::subscriberOptions*>(testing::_))).Times(1).WillRepeatedly(testing::DoAll(testing::Invoke(SensorTest::NsqClientData), testing::Return(true)));
  EXPECT_CALL(m_mockpubsub, ConnectAndStartSubscriber()).WillOnce(testing::Return(true));
  sensor_obj.InitNsq();

  // Mock the register function
  EXPECT_CALL(m_mockpubsub, PublishMessage("ms-cs-core-ctrl-sensor-register", testing::Matcher<folly::fbvector<unsigned char>*>(testing::NotNull()))).Times(testing::AtLeast(1)).WillRepeatedly(::testing::Return(true));
  sensor_obj.RegisterApp();
}

TEST_F(SensorTest, flowMeterInlet) {
  // Mock the init-nsq function
  EXPECT_CALL(m_mockpubsub, CreatePublisher(FLAGS_nsq_ip_port, ms_nsq_client::IMessagingServiceAdapter::E_TCP)).WillOnce(testing::Return(true));
  EXPECT_CALL(m_mockpubsub, Subscribe(::testing::Matcher<ms_nsq_client::IMessagingServiceAdapter::subscriberOptions*>(testing::_))).Times(1).WillRepeatedly(testing::DoAll(testing::Invoke(SensorTest::NsqClientData), testing::Return(true)));
  EXPECT_CALL(m_mockpubsub, ConnectAndStartSubscriber()).WillOnce(testing::Return(true));
  sensor_obj.InitNsq();

  // Mock the publish function
  EXPECT_CALL(m_mockpubsub, PublishMessage("ms-cs-sensor-flow-meter", testing::Matcher<folly::fbvector<unsigned char>*>(testing::NotNull()))).Times(testing::AtLeast(1)).WillRepeatedly(testing::DoAll(testing::Invoke(SensorTest::VerifyFlowMeterInlet), testing::Return(true)));
  struct ms_cs_sensor_flow_meter_embd::SensorData sensor_info;
  sensor_obj.PubTelemetryData(Common::SENSOR_FLOW_METER_INLET, &sensor_info, gSensorData, Common::NOTIFICATION_PERIODIC, Common::SENSOR_STATUS_GOOD);
}

TEST_F(SensorTest, flowMeterOutlet) {
  // Mock the init-nsq function
  EXPECT_CALL(m_mockpubsub, CreatePublisher(FLAGS_nsq_ip_port, ms_nsq_client::IMessagingServiceAdapter::E_TCP)).WillOnce(testing::Return(true));
  EXPECT_CALL(m_mockpubsub, Subscribe(::testing::Matcher<ms_nsq_client::IMessagingServiceAdapter::subscriberOptions*>(testing::_))).Times(1).WillRepeatedly(testing::DoAll(testing::Invoke(SensorTest::NsqClientData), testing::Return(true)));
  EXPECT_CALL(m_mockpubsub, ConnectAndStartSubscriber()).WillOnce(testing::Return(true));
  sensor_obj.InitNsq();

  // Mock the publish function
  EXPECT_CALL(m_mockpubsub, PublishMessage("ms-cs-sensor-flow-meter", testing::Matcher<folly::fbvector<unsigned char>*>(testing::NotNull()))).Times(testing::AtLeast(1)).WillRepeatedly(testing::DoAll(testing::Invoke(SensorTest::VerifyFlowMeterOutlet), testing::Return(true)));
  struct ms_cs_sensor_flow_meter_embd::SensorData sensor_info;
  sensor_obj.PubTelemetryData(Common::SENSOR_FLOW_METER_OUTLET, &sensor_info, gSensorData, Common::NOTIFICATION_PERIODIC, Common::SENSOR_STATUS_GOOD);
}
