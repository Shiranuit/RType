/*
** EPITECH PROJECT, 2020
** B-CPP-501-MPL-5-1-rtype-antoine.maillard
** File description:
** RTypeServer.test
*/

#include <boost/test/unit_test.hpp>
#include "server/Class/RTypeServer/RTypeServer.hpp"

BOOST_AUTO_TEST_SUITE(TestsRTypeServer)

BOOST_AUTO_TEST_CASE(configuration)
{
  RTypeServer *server = new RTypeServer();

  BOOST_TEST(server->getPort() == 0);

  BOOST_TEST(server->configure(3000) == true);

  BOOST_TEST(server->getPort() == 3000);
  if (server) delete server;
}

BOOST_AUTO_TEST_SUITE_END()