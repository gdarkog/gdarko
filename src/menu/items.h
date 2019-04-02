/* -*- C++ -*- */
#pragma once

#include "../menu.h"
#include "out.h"

namespace Menu {

  // template<typename I>
  // struct ItemIterator {
  //   size_t at=0;
  //   virtual inline I operator*() {return data[at];}
  //   virtual inline I operator==(const It o) const {return at==o.at;}
  //   virtual inline I operator!=(const It o) const {return !operator==(o);}
  //   virtual inline I operator++() {return data[++at];}
  //   virtual inline I operator++(I) {return data[at++];}
  // };

  ///////////////////////////////////////////////////////////////
  // menu items -----------------------------------
  struct Item {
    virtual void out(MenuOut& o) {}
    #if (MENU_INJECT_PARTS==true)
      virtual void out(MenuOut& o,PrinterPart& pp) {}
    #endif
    virtual size_t size() {return 1;}
    virtual Item& operator[](size_t) {return *this;}
  };

  //adapt specific types as menu items
  //provide virtual overrides for them
  template<typename O>
  struct Prompt:public virtual Item,public O {
    using O::O;
    using This=Prompt<O>;
    inline void out(MenuOut& o) override {O::out(o);}
    #if (MENU_INJECT_PARTS==true)
      void out(MenuOut& o,PrinterPart& pp) override;
    #endif
    size_t size() override {return O::size();}
    Item& operator[](size_t n) override {return O::operator[](n);}
    template<template<typename> class T>
    //type injection
    inline void stack(MenuOut& o) {Prompt<T<O>>(*this).out(o);}
  };

  #if (MENU_INJECT_PARTS==true)
    struct PrinterPart {
      template<typename O>
      void use(MenuOut& o,Prompt<O>& i) {
        Serial<<"PrinterPart::use..."<<endl;
        Prompt<O>(i).out(o);
        // i.stack<part>(o);
      }
    };
  #endif

  #if (MENU_INJECT_PARTS==true)
    template<typename O>
    void Prompt<O>::out(MenuOut& o,PrinterPart& pp) {
      pp.use<O>(o,*this);
    }
  #endif

  //static composition blocks -----------------------
  struct Empty {
    inline Empty() {}
    inline Empty(Empty&) {}
    static inline void out(MenuOut&) {}
    static inline size_t size() {return 1;}
    inline Item& operator[](size_t n) {return *reinterpret_cast<Item*>(this);}
  };

  template<typename O>
  struct Text:public O {
    const char* text;
    inline Text(const char* text):text(text) {}
    inline Text(const Text<O>& t):text(t.text) {}
    inline void out(MenuOut &o) {o<<text;}
  };

  template<size_t n,typename O>
  class StaticMenu:public O {
  protected:
    Item* data[n];
  public:
    template<typename... OO>
    inline StaticMenu(OO... oo):data{oo...} {}
    template<typename... OO>
    inline StaticMenu(const char*title,OO... oo):O(title),data{oo...} {}
    static inline size_t size() {return n;}
    inline Item& operator[](size_t i) const {return *data[i];}
  };

};//Menu
