#include "PID_tests.h"

using namespace ::testing;

TEST( PID, get_actuator )
    {
    G_DEVICE_MANAGER()->clear_io_devices();

    auto res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_REGULATOR, device::DST_REGULATOR_PID, "TC1",
        "Test controller", "T" );
    ASSERT_EQ( nullptr, res );
    auto p1_dev = G_DEVICE_MANAGER()->get_C( "TC1" );
    ASSERT_NE( nullptr, p1_dev );
    auto p1 = dynamic_cast<PID*>( p1_dev );
    ASSERT_NE( nullptr, p1 );
    
    ASSERT_EQ( nullptr, p1->get_actuator() );

    p1->set_string_property( "OUT_VALUE", "TC1" );
    auto m = p1->get_actuator();
    ASSERT_EQ( nullptr, m );

    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_M, device::DST_M_VIRT, "M1", "Test motor", "M" );
    ASSERT_EQ( nullptr, res );
    auto m1 = G_DEVICE_MANAGER()->get_M( "M1" );
    ASSERT_NE( nullptr, m1 );
    p1->set_string_property( "OUT_VALUE", "M1" );
    m = p1->get_actuator();
    ASSERT_NE( nullptr, m );
    ASSERT_EQ( m1, m );
    }

TEST( PID, direct_set_value )
    {
    G_DEVICE_MANAGER()->clear_io_devices();

    auto res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_REGULATOR, device::DST_REGULATOR_PID, "TC1",
        "Test controller", "T" );
    ASSERT_EQ( nullptr, res );        
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_TE, device::DST_TE_VIRT, "TE1", "Test sensor", "T" );
    ASSERT_EQ( nullptr, res );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_M, device::DST_M_VIRT, "M1", "Test motor", "M" );
    ASSERT_EQ( nullptr, res );
    G_DEVICE_MANAGER()->print();

    auto m1 = G_DEVICE_MANAGER()->get_M( "M1" );
    ASSERT_NE( nullptr, m1 );
    auto p1_dev = G_DEVICE_MANAGER()->get_C( "TC1" );
    ASSERT_NE( nullptr, p1_dev );
    PID* p1 = dynamic_cast<PID*>( p1_dev );
    ASSERT_NE( nullptr, p1 );

    p1->set_string_property( "IN_VALUE", "TE1" );
    p1->set_string_property( "OUT_VALUE", "M1" );

    //PID switched off.
    p1_dev->set_value( 10 );
    EXPECT_EQ( .0f, m1->get_value() );
    EXPECT_EQ( .0f, m1->get_state() );

    //PID switched on.
    p1_dev->on();
    p1_dev->set_value( 10 );
    EXPECT_EQ( .0f, m1->get_value() );
    EXPECT_EQ( 1, m1->get_state() );

    //PID switched off.
    p1_dev->off();
    EXPECT_EQ( .0f, m1->get_value() );
    EXPECT_EQ( 0, m1->get_state() );
    }

int reverse( char* str )
    {
    auto l1 = strlen( str ) / 2;
    auto l2 = l1 + strlen( str ) % 2;
    for ( auto i = 0u, j = l2; i < l1; i++, j++ )
        {
        std::swap( str[ i ], str[ j ] );
        }

    return 0;
    }

int reverse2( char* str )
    {
    auto l = strlen( str );
    for ( auto i = 0u; i + 1 < l; i += 2 )
        {
        std::swap( str[ i ], str[ i + 1 ] );
        }

    return 0;
    }

TEST( PID, direct_set_value1 )
    {
    char t1[] = "weathe";
    reverse( t1 );

    char t2[] = "weather";
    reverse( t2 );

    char t3[] = "";
    reverse( t3 );
    reverse2( t3 );


    char t4[] = "123456789";
    reverse2( t4 );
    }


  struct ListNode {
      int val;
      ListNode *next;
      ListNode() : val(0), next(nullptr) {}
      ListNode(int x) : val(x), next(nullptr) {}
      ListNode(int x, ListNode *next) : val(x), next(next) {}
  };
 

  class Solution
      {
      public:
          ListNode* middleNode( ListNode* head ) {

              auto l = 1;
              auto p = head;
              while ( p->next )
                  {
                  p = p->next;
                  l++;
                  }

              l = l / 2;
              while ( l )
                  {
                  head = head->next;
                  l--;
                  }

              return head;
              }
      };

  TEST( PID, direct_set_value2 )
      {
      ListNode ls( 1, nullptr );
      //ls.next = new ListNode( 2, nullptr );
      //ls.next->next = new ListNode( 3, nullptr );

      ListNode* p = &ls;


      Solution s;
      s.middleNode( p );
      }