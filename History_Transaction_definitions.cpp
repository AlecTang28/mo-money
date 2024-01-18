#include <cassert>
#include <iomanip>
#include <iostream>

#include "project4.hpp"
#include "History.hpp"
#include "Transaction.hpp"

////////////////////////////////////////////////////////////////////////////////
// Definitions for Transaction class
////////////////////////////////////////////////////////////////////////////////
//
//

Transaction::Transaction(std::string ticker_symbol, unsigned int day_date,
unsigned int month_date, unsigned year_date, bool buy_sell_trans, 
unsigned int number_shares, double trans_amount) :

  symbol{ticker_symbol},
  day{day_date},
  month{month_date},
  year{year_date},
  trans_type{},
  shares{number_shares},
  amount{trans_amount},
  trans_id{assigned_trans_id},
  p_next{nullptr},
  acb{0},
  cgl{0},
  acb_per_share{0}

{
  if (buy_sell_trans)
    trans_type = "Buy";
  else
    trans_type = "Sell";

  assigned_trans_id++;
}

Transaction::~Transaction()
{
  delete p_next;
  p_next = nullptr;
}

bool Transaction::operator<(Transaction const &other)
{

  if (year < other.year)
    return true;
  else if (year == other.year)
  {

    if (month < other.month)
      return true;
    else if (month == other.month)
    {

      if (day < other.day)
        return true;
      else if (day == other.day)
      {

        if (trans_id > other.trans_id)
          return true;
        else
          return false;
      }
    }
  }

  return false;
}

// GIVEN
// Member functions to get values.
//
std::string Transaction::get_symbol() const { return symbol; }
unsigned int Transaction::get_day() const { return day; }
unsigned int Transaction::get_month() const { return month; }
unsigned int Transaction::get_year() const { return year; }
unsigned int Transaction::get_shares() const { return shares; }
double Transaction::get_amount() const { return amount; }
double Transaction::get_acb() const { return acb; }
double Transaction::get_acb_per_share() const { return acb_per_share; }
unsigned int Transaction::get_share_balance() const { return share_balance; }
double Transaction::get_cgl() const { return cgl; }
bool Transaction::get_trans_type() const { return (trans_type == "Buy") ? true : false; }
unsigned int Transaction::get_trans_id() const { return trans_id; }
Transaction *Transaction::get_next() { return p_next; }

// GIVEN
// Member functions to set values.
//
void Transaction::set_acb(double acb_value) { acb = acb_value; }
void Transaction::set_acb_per_share(double acb_share_value) { acb_per_share = acb_share_value; }
void Transaction::set_share_balance(unsigned int bal) { share_balance = bal; }
void Transaction::set_cgl(double value) { cgl = value; }
void Transaction::set_next(Transaction *p_new_next) { p_next = p_new_next; }

// GIVEN
// Print the transaction.
//
void Transaction::print()
{
  std::cout << std::fixed << std::setprecision(2);
  std::cout << std::setw(4) << get_trans_id() << " "
            << std::setw(4) << get_symbol() << " "
            << std::setw(4) << get_day() << " "
            << std::setw(4) << get_month() << " "
            << std::setw(4) << get_year() << " ";

  if (get_trans_type())
  {
    std::cout << "  Buy  ";
  }
  else
  {
    std::cout << "  Sell ";
  }

  std::cout << std::setw(4) << get_shares() << " "
            << std::setw(10) << get_amount() << " "
            << std::setw(10) << get_acb() << " " << std::setw(4) << get_share_balance() << " "
            << std::setw(10) << std::setprecision(3) << get_acb_per_share() << " "
            << std::setw(10) << std::setprecision(3) << get_cgl()
            << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
// Definitions for the History class
////////////////////////////////////////////////////////////////////////////////
//
//

History::History()
{
  p_head = nullptr;
}

History::~History()
{
  delete p_head;
  p_head = nullptr;
  
}

void History::read_history()
{

  ece150::open_file();

  while (ece150::next_trans_entry() == true)
  {

    Transaction *new_transaction = new Transaction{
        ece150::get_trans_symbol(), ece150::get_trans_day(),
        ece150::get_trans_month(), ece150::get_trans_year(),
        ece150::get_trans_type(), ece150::get_trans_shares(),
        ece150::get_trans_amount()};

    insert(new_transaction);
  }

  ece150::close_file();
}

void History::insert(Transaction *p_new_trans)
{

  if (this->p_head == nullptr)
  {
    p_head = p_new_trans;
  }

  else
  {

    Transaction *prev_entry = p_head;

    while (prev_entry->get_next() != nullptr)
    {
      prev_entry = prev_entry->get_next();
    }
    prev_entry->set_next(p_new_trans);
  }
}

void History::print()
{

  std::cout << "========== BEGIN TRANSACTION HISTORY ============" << std::endl;

  Transaction *header = get_p_head();
  
  while(header != nullptr)
  {
      header->print();
      header = header->get_next();
  }

  std::cout << "========== END TRANSACTION HISTORY ============" << std::endl;
}

void History::sort_by_date()
{

  Transaction *first = nullptr;
  Transaction *second = nullptr;
  Transaction *temp;

  Transaction* new_head = nullptr;

  while(p_head != nullptr)
  {

    temp = p_head;
    p_head = p_head->get_next();
    temp->set_next(nullptr);

    if(new_head == nullptr){
      new_head=temp;
      continue;
    }
    // insert to head
    if(*temp < *new_head){
      temp->set_next(new_head);
      new_head = temp;
      continue;
    }

    // insert to tail/middle
    first = new_head;
    second = new_head->get_next();

    while((second != nullptr) && (*second < *temp)){
      first = second;
      second = second->get_next();
    }

    if(second == nullptr){
      // insert to tail
      first->set_next(temp);
    }
    else{
      first->set_next(temp);
      temp->set_next(second);
    }
  }

  p_head = new_head;

}

void History::update_acb_cgl()
{

  Transaction *header = p_head;
  Transaction *temp = p_head;

  header->set_acb(header->get_amount() );
  header->set_share_balance(header->get_shares() );
  header->set_acb_per_share(header->get_acb() / header->get_share_balance() );

  while (header->get_next() != nullptr)
  {
    temp = header;
    header = header->get_next();

    //if buy
    if(header->get_trans_type() ){
      header->set_acb(temp->get_acb() + header->get_amount() );
      header->set_share_balance(temp->get_share_balance() + header->get_shares() );
      header->set_acb_per_share(header->get_acb() / header->get_share_balance() ); 
      header->set_cgl(0);
    }//if sell
    else{
      header->set_acb(temp->get_acb() - ( (temp->get_acb() / temp->get_share_balance() ) * header->get_shares() ) );
      header->set_share_balance(temp->get_share_balance() - header->get_shares() );
      header->set_acb_per_share(header->get_acb() / header->get_share_balance() );
      header->set_cgl(header->get_amount() - (header->get_shares() * temp->get_acb_per_share() ) );
    }
  }
}

double History::compute_cgl(unsigned int year)
{
  Transaction *trans = p_head;
  double money{0};

  while(trans != nullptr){
    if(trans->get_year() == year){
      money += trans->get_cgl();
    }
    
    trans = trans->get_next();
  }

  return money;

}

Transaction *History::get_p_head() { return p_head; }