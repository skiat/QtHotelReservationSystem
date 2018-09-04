#include "mainwindow.h"
#include "reservation.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <mongocxx/collection.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <mongocxx/exception/exception.hpp>
#include <QDebug>
#include <QTextCursor>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::basic::kvp;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mongoStuff = std::make_unique<MongoStuff>();
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_insertButton_clicked()
{
    qWarning("About to insert 4 reservations");
    try {
        Reservation firstBooking{ mongoStuff->read_date("2018-8-1",0), mongoStuff->read_date("2018-8-3",0), "George Clooney", 2 };
        Reservation secondBooking{ mongoStuff->read_date("2018-8-6",0), mongoStuff->read_date("2018-8-9",0), "George Clooney", 3 };
        Reservation thirdBooking{ mongoStuff->read_date("2018-8-12",0), mongoStuff->read_date("2018-8-18",0), "George Clooney", 1 };
        Reservation fourthBooking{ mongoStuff->read_date("2018-8-2",0), mongoStuff->read_date("2018-8-7",0), "George Clooney", 2 };
        qWarning("Created the reservation objects");
        mongocxx::collection collection = mongoStuff->db["reservations"];
        qWarning("Got the collection");
        mongoStuff->addReservation(collection, firstBooking);
        mongoStuff->addReservation(collection, secondBooking);
        mongoStuff->addReservation(collection, thirdBooking);
        mongoStuff->addReservation(collection, fourthBooking);
        qWarning("Inserted 4 reservations");
    }  catch (const mongocxx::exception &ex) {
        qWarning() << "Hit an exception..." << ex.what();
    }
}



void MainWindow::on_connectButton_clicked()
{
    qWarning() << "connectButton clicked";
    mongoStuff->connectToDb();
}

void MainWindow::on_readButton_clicked()
{
    try {
        mongocxx::collection collection = mongoStuff->db["reservations"];
        qWarning("Got the collection");
        auto stuff = mongoStuff->readAllReservations(collection);
        for ( auto s : stuff ) {
            ui->textEdit->append (QString::fromStdString(s));
        }
    }  catch (const mongocxx::exception &ex) {
        qWarning() << "Hit an exception..." << ex.what();
    }
}
