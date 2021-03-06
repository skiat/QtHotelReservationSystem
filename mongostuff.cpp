#include "mongostuff.h"
#include <QDebug>
#include "mongocxx/instance.hpp"
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/client.hpp>
#include <bsoncxx/view_or_value.hpp>
#include <sstream>
#include <iomanip>

MongoStuff::MongoStuff()
{
    qWarning() << "Constructing MongoStuff";
}


void MongoStuff::connectToDb() {
    qWarning() << "Connecting to the mongodb server...\n";
    try {
        db = client["local"];
        qWarning() << "got db...\n";
    }  catch (const mongocxx::exception &ex) {
        qWarning() << "Hit an exception: " << ex.what();
    }
}

void MongoStuff::addReservation(mongocxx::collection & collection,  const Reservation & r) {
    qWarning() << "Inserting a reservation";
    auto bson = r.toBson();
    qWarning() << "Got the bson object";
    bsoncxx::stdx::optional<mongocxx::result::insert_one> result = collection.insert_one(bson);
    qWarning() << "Insert done";
}

std::vector<Reservation> MongoStuff::readAllReservations(mongocxx::collection & collection) {
    qWarning() << "Reading all reservations";

    //bsoncxx::stdx::optional<mongocxx::result::find> result = collection.find();

    std::vector<Reservation> reservations {};
    mongocxx::cursor cursor = collection.find({});
    for(auto doc : cursor) {
        auto sd_view = doc["startDate"].get_date();
        auto ed_view = doc["endDate"].get_date();
        auto view = doc["client"].get_utf8().value;
        auto guests_view = doc["numberOfGuests"].get_int32();
        std::string name = view.to_string();
        Reservation r {sd_view,ed_view,name,guests_view};
        //qWarning() << QString::fromStdString(name);
        reservations.push_back(r);
    }
    return reservations;
}

/**
* @brief Convert a Y-m-d to a bsoncxx date
* @see https://stackoverflow.com/questions/42148535/mongodb-query-across-a-date-range
*/
bsoncxx::types::b_date MongoStuff::read_date(const std::string& date, std::int32_t offset_from_utc) {
    std::tm utc_tm{};
    std::istringstream ss{ date };

    // Read time into std::tm.
    ss >> std::get_time(&utc_tm, "%Y-%m-%d");

    // Convert std::tm to std::time_t.
    std::time_t utc_time = std::mktime(&utc_tm);

    // Convert std::time_t std::chrono::systemclock::time_point.
    std::chrono::system_clock::time_point time_point =
            std::chrono::system_clock::from_time_t(utc_time);

    return bsoncxx::types::b_date{ time_point +
                std::chrono::hours{ offset_from_utc } };
}
