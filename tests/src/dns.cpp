#include <gtest/gtest.h>
#include <iostream>
#include "dns.h"
#include "ipv6_address.h"
#include "utils.h"

using namespace Tins;


class DNSTest : public testing::Test {
public:
    static const uint8_t expected_packet[];
    
    void test_equals(const DNS &dns1, const DNS &dns2);
    void test_equals(const DNS::Query &q1, const DNS::Query &q2);
    void test_equals(const DNS::Resource &q1, const DNS::Resource &q2);
};

const uint8_t DNSTest::expected_packet[] = {
    0, 19, 215, 154, 0, 1, 0, 1, 0, 0, 0, 0, 3, 119, 119, 119, 7, 101, 
    120, 97, 109, 112, 108, 101, 3, 99, 111, 109, 0, 0, 1, 0, 1, 3, 119, 
    119, 119, 7, 101, 120, 97, 109, 112, 108, 101, 3, 99, 111, 109, 0, 
    0, 1, 0, 1, 0, 0, 18, 52, 0, 4, 192, 168, 0, 1
};



void DNSTest::test_equals(const DNS &dns1, const DNS &dns2) {
    EXPECT_EQ(dns1.id(), dns2.id());
    EXPECT_EQ(dns1.type(), dns2.type());
    EXPECT_EQ(dns1.opcode(), dns2.opcode());
    EXPECT_EQ(dns1.authoritative_answer(), dns2.authoritative_answer());
    EXPECT_EQ(dns1.truncated(), dns2.truncated());
    EXPECT_EQ(dns1.recursion_desired(), dns2.recursion_desired());
    EXPECT_EQ(dns1.recursion_available(), dns2.recursion_available());
    EXPECT_EQ(dns1.z(), dns2.z());
    EXPECT_EQ(dns1.authenticated_data(), dns2.authenticated_data());
    EXPECT_EQ(dns1.checking_disabled(), dns2.checking_disabled());
    EXPECT_EQ(dns1.rcode(), dns2.rcode());
    EXPECT_EQ(dns1.questions_count(), dns2.questions_count());
    EXPECT_EQ(dns1.answers_count(), dns2.answers_count());
    EXPECT_EQ(dns1.authority_count(), dns2.authority_count());
    EXPECT_EQ(dns1.additional_count(), dns2.additional_count());
    EXPECT_EQ(dns1.pdu_type(), dns2.pdu_type());
    EXPECT_EQ(dns1.header_size(), dns2.header_size());
    EXPECT_EQ(bool(dns1.inner_pdu()), bool(dns2.inner_pdu()));
}

void DNSTest::test_equals(const DNS::Query &q1, const DNS::Query &q2) {
    EXPECT_EQ(q1.dname(), q2.dname());
    EXPECT_EQ(q1.type(), q2.type());
    EXPECT_EQ(q1.query_class(), q2.query_class());
}

void DNSTest::test_equals(const DNS::Resource &q1, const DNS::Resource &q2) {
    EXPECT_EQ(q1.dname(), q2.dname());
    EXPECT_EQ(q1.data(), q2.data());
    EXPECT_EQ(q1.type(), q2.type());
    EXPECT_EQ(q1.query_class(), q2.query_class());
    EXPECT_EQ(q1.ttl(), q2.ttl());
}

TEST_F(DNSTest, ConstructorFromBuffer) {
    DNS dns(expected_packet, sizeof(expected_packet));
    // id=0x13, qr=1, opcode=0xa, aa=1, tc=1, rd=1, ra=1, z=0, rcode=0xa
    EXPECT_EQ(dns.id(), 0x13);
    EXPECT_EQ(dns.type(), DNS::RESPONSE);
    EXPECT_EQ(dns.opcode(), 0xa);
    EXPECT_EQ(dns.authoritative_answer(), 1);
    EXPECT_EQ(dns.truncated(), 1);
    EXPECT_EQ(dns.recursion_desired(), 1);
    EXPECT_EQ(dns.recursion_available(), 1);
    EXPECT_EQ(dns.z(), 0);
    EXPECT_EQ(dns.rcode(), 0xa);
    EXPECT_EQ(dns.questions_count(), 1);
    EXPECT_EQ(dns.answers_count(), 1);
    
    std::list<DNS::Query> queries = dns.queries();
    ASSERT_EQ(queries.size(), 1U);
    test_equals(queries.front(), DNS::Query("www.example.com", DNS::A, DNS::IN));
    
    std::list<DNS::Resource> answers = dns.answers();
    ASSERT_EQ(answers.size(), 1U);
    test_equals(answers.front(), DNS::Resource("www.example.com", "192.168.0.1", DNS::A, DNS::IN, 0x1234));
}

TEST_F(DNSTest, Serialization) {
    DNS dns(expected_packet, sizeof(expected_packet));
    DNS::serialization_type buffer = dns.serialize();
    ASSERT_EQ(buffer.size(), sizeof(expected_packet));
    EXPECT_TRUE(std::equal(buffer.begin(), buffer.end(), expected_packet));
}

TEST_F(DNSTest, CopyConstructor) {
    DNS dns1(expected_packet, sizeof(expected_packet));
    DNS dns2(dns1);
    test_equals(dns1, dns2);
}

TEST_F(DNSTest, CopyAssignmentOperator) {
    DNS dns1(expected_packet, sizeof(expected_packet));
    DNS dns2;
    dns2 = dns1;
    test_equals(dns1, dns2);
}

TEST_F(DNSTest, NestedCopy) {
    DNS *nested = new DNS(expected_packet, sizeof(expected_packet));
    DNS dns1(expected_packet, sizeof(expected_packet));
    dns1.inner_pdu(nested);
    DNS dns2(dns1);
    test_equals(dns1, dns2);
    dns2.inner_pdu(0);
    dns2 = dns1;
    test_equals(dns1, dns2);
}

TEST_F(DNSTest, ID) {
    DNS dns;
    dns.id(0x7263);
    EXPECT_EQ(dns.id(), 0x7263);
}

TEST_F(DNSTest, Type) {
    DNS dns;
    dns.type(DNS::RESPONSE);
    EXPECT_EQ(dns.type(), DNS::RESPONSE);
}

TEST_F(DNSTest, Opcode) {
    DNS dns;
    dns.opcode(0xa);
    EXPECT_EQ(dns.opcode(), 0xa);
}

TEST_F(DNSTest, AuthoritativeAnswer) {
    DNS dns;
    dns.authoritative_answer(1);
    EXPECT_EQ(dns.authoritative_answer(), 1);
}

TEST_F(DNSTest, Truncated) {
    DNS dns;
    dns.truncated(1);
    EXPECT_EQ(dns.truncated(), 1);
}

TEST_F(DNSTest, RecursionDesired) {
    DNS dns;
    dns.recursion_desired(1);
    EXPECT_EQ(dns.recursion_desired(), 1);
}

TEST_F(DNSTest, RecursionAvailable) {
    DNS dns;
    dns.recursion_available(1);
    EXPECT_EQ(dns.recursion_available(), 1);
}

TEST_F(DNSTest, Z) {
    DNS dns;
    dns.z(1);
    EXPECT_EQ(dns.z(), 1);
}

TEST_F(DNSTest, AuthenticatedData) {
    DNS dns;
    dns.authenticated_data(1);
    EXPECT_EQ(dns.authenticated_data(), 1);
}

TEST_F(DNSTest, CheckingDisabled) {
    DNS dns;
    dns.checking_disabled(1);
    EXPECT_EQ(dns.checking_disabled(), 1);
}

TEST_F(DNSTest, RCode) {
    DNS dns;
    dns.rcode(0xa);
    EXPECT_EQ(dns.rcode(), 0xa);
}

TEST_F(DNSTest, Question) {
    DNS dns;
    dns.add_query(DNS::Query("www.example.com", DNS::A, DNS::IN));
    dns.add_query(DNS::Query("www.example2.com", DNS::MX, DNS::IN));
    ASSERT_EQ(dns.questions_count(), 2);
    
    DNS::queries_type queries(dns.queries());
    for(DNS::queries_type::const_iterator it = queries.begin(); it != queries.end(); ++it) {
        EXPECT_TRUE(it->dname() == "www.example.com" || it->dname() == "www.example2.com");
        if(it->dname() == "www.example.com") {
            EXPECT_EQ(it->type(), DNS::A);
            EXPECT_EQ(it->query_class(), DNS::IN);
        }
        else if(it->dname() == "www.example2.com") {
            EXPECT_EQ(it->type(), DNS::MX);
            EXPECT_EQ(it->query_class(), DNS::IN);
        }
    }
}

TEST_F(DNSTest, Answers) {
    DNS dns;
    dns.add_answer("www.example.com", DNS::make_info(DNS::A, DNS::IN, 0x762), IPv4Address("127.0.0.1"));
    dns.add_answer("www.example2.com", DNS::make_info(DNS::MX, DNS::IN, 0x762), std::string("mail.example.com"));
    ASSERT_EQ(dns.answers_count(), 2);
    
    DNS::resources_type resources = dns.answers();
    for(DNS::resources_type::const_iterator it = resources.begin(); it != resources.end(); ++it) {
        EXPECT_TRUE(it->dname() == "www.example.com" || it->dname() == "www.example2.com");
        if(it->dname() == "www.example.com") {
            EXPECT_EQ(it->type(), DNS::A);
            EXPECT_EQ(it->ttl(), 0x762U);
            EXPECT_EQ(it->data(), "127.0.0.1");
            EXPECT_EQ(it->query_class(), DNS::IN);
        }
        else if(it->dname() == "www.example2.com") {
            EXPECT_EQ(it->type(), DNS::MX);
            EXPECT_EQ(it->ttl(), 0x762U);
            EXPECT_EQ(it->data(), "mail.example.com");
            EXPECT_EQ(it->query_class(), DNS::IN);
        }
    }
}

TEST_F(DNSTest, AnswersWithSameName) {
    DNS dns;
    dns.add_answer("www.example.com", DNS::make_info(DNS::A, DNS::IN, 0x762), IPv4Address("127.0.0.1"));
    dns.add_answer("www.example.com", DNS::make_info(DNS::A, DNS::IN, 0x762), IPv4Address("127.0.0.2"));
    ASSERT_EQ(dns.answers_count(), 2);
    DNS::resources_type resources = dns.answers();
    for(DNS::resources_type::const_iterator it = resources.begin(); it != resources.end(); ++it) {
        EXPECT_TRUE(it->data() == "127.0.0.1" || it->data() == "127.0.0.2");
        EXPECT_EQ(it->dname(), "www.example.com");
        EXPECT_EQ(it->type(), DNS::A);
        EXPECT_EQ(it->ttl(), 0x762U);
        EXPECT_EQ(it->query_class(), DNS::IN);
    }
}

TEST_F(DNSTest, AnswersV6) {
    DNS dns;
    dns.add_answer("www.example.com", DNS::make_info(DNS::AAAA, DNS::IN, 0x762), IPv6Address("f9a8:239::1:1"));
    dns.add_answer("www.example.com", DNS::make_info(DNS::AAAA, DNS::IN, 0x762), IPv6Address("f9a8:239::1:1"));
    ASSERT_EQ(dns.answers_count(), 2);
    
    DNS::resources_type resources = dns.answers();
    for(DNS::resources_type::const_iterator it = resources.begin(); it != resources.end(); ++it) {
        EXPECT_EQ(it->dname(), "www.example.com");
        EXPECT_EQ(it->type(), DNS::AAAA);
        EXPECT_EQ(it->ttl(), 0x762U);
        EXPECT_EQ(it->data(), "f9a8:239::1:1");
        EXPECT_EQ(it->query_class(), DNS::IN);
    }
}
