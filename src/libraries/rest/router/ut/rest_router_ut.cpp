// Copyright (C) 2024-2025 Aleksey Loginov
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Home page: https://github.com/Just-Queue-it/JustQueueIt/

#include <doctest/doctest.h>

#include <libraries/rest/router/rest_router.hpp>

struct SerializableData
{
    int                      data{};
    std::vector<std::string> texts{};

    auto operator<=>(const SerializableData& rhs) const = default;
};

TEST_CASE("Router provide correct routing")
{
    rest::Router router{};
    SUBCASE("get empty")
    {
        REQUIRE(router.Route(rest::Request{.method = rest::Request::Method::GET, .content_type = rest::ContentType::TEXT_PLAIN}).status_code == rest::Response::Status::NOT_FOUND);
    }

    SUBCASE("get /test")
    {
        REQUIRE(router.Route(rest::Request{.method = rest::Request::Method::GET, .path = "/test", .content_type = rest::ContentType::TEXT_PLAIN}).status_code == rest::Response::Status::NOT_FOUND);
    }

    SUBCASE("add get /test")
    {
        router.AddRoute("/test", rest::Request::Method::GET, [](const rest::Request&, const rest::Router::Params&) { return rest::Response{.status_code = rest::Response::Status::OK, .content_type = rest::ContentType::TEXT_PLAIN}; });
        SUBCASE("get /test")
        {
            REQUIRE(router.Route(rest::Request{.method = rest::Request::Method::GET, .path = "/test", .content_type = rest::ContentType::TEXT_PLAIN}).status_code == rest::Response::Status::OK);
        }

        SUBCASE("get /test_2")
        {
            REQUIRE(router.Route(rest::Request{.method = rest::Request::Method::GET, .path = "/test_2", .content_type = rest::ContentType::TEXT_PLAIN}).status_code == rest::Response::Status::NOT_FOUND);
        }

        SUBCASE("post /test")
        {
            REQUIRE(router.Route(rest::Request{.method = rest::Request::Method::POST, .path = "/test", .content_type = rest::ContentType::TEXT_PLAIN}).status_code == rest::Response::Status::METHOD_NOT_ALLOWED);
        }
        SUBCASE("add post /test")
        {
            router.AddRoute("/test", rest::Request::Method::POST, [](const rest::Request&, const rest::Router::Params&) { return rest::Response{.status_code = rest::Response::Status::PROCESSING, .content_type = rest::ContentType::TEXT_PLAIN}; });
            SUBCASE("get /test")
            {
                REQUIRE(router.Route(rest::Request{.method = rest::Request::Method::GET, .path = "/test", .content_type = rest::ContentType::TEXT_PLAIN}).status_code == rest::Response::Status::OK);
            }

            SUBCASE("post /test")
            {
                REQUIRE(router.Route(rest::Request{.method = rest::Request::Method::POST, .path = "/test", .content_type = rest::ContentType::TEXT_PLAIN}).status_code == rest::Response::Status::PROCESSING);
            }
        }
    }
    SUBCASE("exception inside handler")
    {
        router.AddRoute("/test", rest::Request::Method::GET, [](const rest::Request&, const rest::Router::Params&) -> rest::Response { throw std::runtime_error("test"); });
        REQUIRE(router.Route(rest::Request{.method = rest::Request::Method::GET, .path = "/test", .content_type = rest::ContentType::TEXT_PLAIN}).status_code == rest::Response::Status::INTERNAL_SERVER_ERROR);
    }
    SUBCASE("pattern with parameter")
    {
        router.AddRoute("/test/{:id}/subtest", rest::Request::Method::GET, [](const rest::Request&, const rest::Router::Params& params) {
            REQUIRE(params.at("id") == "135");
            return rest::Response{.status_code = rest::Response::Status::OK, .content_type = rest::ContentType::TEXT_PLAIN};
        });
        REQUIRE(router.Route(rest::Request{.method = rest::Request::Method::GET, .path = "/test/135/subtest", .content_type = rest::ContentType::TEXT_PLAIN}).status_code == rest::Response::Status::OK);
        REQUIRE(router.Route(rest::Request{.method = rest::Request::Method::GET, .path = "/test/135", .content_type = rest::ContentType::TEXT_PLAIN}).status_code == rest::Response::Status::NOT_FOUND);
    }
    SUBCASE("query params")
    {
        router.AddRoute("/test/", rest::Request::Method::GET, [](const rest::Request&, const rest::Router::Params& params) {
            REQUIRE(params.at("key") == "value");
            return rest::Response{.status_code = rest::Response::Status::OK, .content_type = rest::ContentType::TEXT_PLAIN};
        });
        REQUIRE(router.Route(rest::Request{.method = rest::Request::Method::GET, .path = "/test/?key=value", .content_type = rest::ContentType::TEXT_PLAIN}).status_code == rest::Response::Status::OK);
    }
    SUBCASE("query params and path params")
    {
        router.AddRoute("/test/{:id}/subtest", rest::Request::Method::GET, [](const rest::Request&, const rest::Router::Params& params) {
            REQUIRE(params.at("key") == "value");
            REQUIRE(params.at("key2") == "value2");
            REQUIRE(params.at("id") == "23");
            return rest::Response{.status_code = rest::Response::Status::OK, .content_type = rest::ContentType::TEXT_PLAIN};
        });
        REQUIRE(router.Route(rest::Request{.method = rest::Request::Method::GET, .path = "/test/23/subtest?key=value&key2=value2", .content_type = rest::ContentType::TEXT_PLAIN}).status_code == rest::Response::Status::OK);
    }
    SUBCASE("check serialize/deserialize")
    {
        const auto data = SerializableData{.data = 30, .texts = {"hello", "world"}};
        const auto str  = rest::Serialize(data, rest::ContentType::APPLICATION_JSON);
        CHECK(str == R"({"data":30,"texts":["hello","world"]})");
        CHECK(rest::DeSerialize<SerializableData>(str, rest::ContentType::APPLICATION_JSON) == data);
    }
    SUBCASE("route with custom serializing")
    {
        auto test = [&] {
            SUBCASE("valid request")
            {
                const auto res = router.Route(rest::Request{.method = rest::Request::Method::GET, .path = "/test", .body = R"({"data": 30, "texts" : ["hello", "world"]})", .content_type = rest::ContentType::APPLICATION_JSON});
                CHECK(res.status_code == rest::Response::Status::OK);
                CHECK(res.content_type == rest::ContentType::APPLICATION_JSON);
                CHECK(res.body == R"({"data":30,"texts":["hello","world"]})");
            }
            SUBCASE("invalid input json")
            {
                const auto res = router.Route(rest::Request{.method = rest::Request::Method::GET, .path = "/test", .body = R"({"data": 20, )", .content_type = rest::ContentType::APPLICATION_JSON});
                CHECK(res.status_code == rest::Response::Status::BAD_REQUEST);
                CHECK(res.body == "Could not parse document");
            }
            SUBCASE("not all required fields")
            {
                const auto res = router.Route(rest::Request{.method = rest::Request::Method::GET, .path = "/test", .body = R"({"texts" : ["hello", "world"]})", .content_type = rest::ContentType::APPLICATION_JSON});
                CHECK(res.status_code == rest::Response::Status::BAD_REQUEST);
                CHECK(res.body == "Field named 'data' not found.");
            }
            SUBCASE("Unsupported content-type request")
            {
                const auto res = router.Route(rest::Request{.method = rest::Request::Method::GET, .path = "/test", .body = R"({"data": 30, "texts" : ["hello", "world"]})", .content_type = rest::ContentType::TEXT_PLAIN});
                CHECK(res.status_code == rest::Response::Status::BAD_REQUEST);
                CHECK(res.content_type == rest::ContentType::TEXT_PLAIN);
                CHECK(res.body == "Unsupported request content type");
            }
            SUBCASE("Unsupported accept-content-type request")
            {
                const auto res = router.Route(rest::Request{.method = rest::Request::Method::GET, .path = "/test", .body = R"({"data": 30, "texts" : ["hello", "world"]})", .content_type = rest::ContentType::APPLICATION_JSON, .accept_content_type = rest::ContentType::TEXT_PLAIN});
                CHECK(res.status_code == rest::Response::Status::BAD_REQUEST);
                CHECK(res.content_type == rest::ContentType::TEXT_PLAIN);
                CHECK(res.body == "Unsupported accept content type");
            }
        };
        SUBCASE("explicit serialization")
        {
            router.AddRoute("/test", rest::Request::Method::GET, [](const SerializableData& request, const rest::Router::Params&) {
                CHECK(request.data == 30);
                CHECK(request.texts.size() == 2);
                CHECK(request.texts[0] == "hello");
                CHECK(request.texts[1] == "world");
                return rest::Router::SerializableResponse<SerializableData>{.status_code = rest::Response::Status::OK, .body = SerializableData{.data = 30, .texts = {"hello", "world"}}};
            });
            test();
        }
        SUBCASE("lean serialization")
        {
            router.AddRoute("/test", rest::Request::Method::GET, [](const SerializableData& request, const rest::Router::Params&) {
                CHECK(request.data == 30);
                CHECK(request.texts.size() == 2);
                CHECK(request.texts[0] == "hello");
                CHECK(request.texts[1] == "world");
                return SerializableData{.data = 30, .texts = {"hello", "world"}};
            });
            test();
        }
    }
}
