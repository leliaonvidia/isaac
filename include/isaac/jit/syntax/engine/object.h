/* Copyright 2015-2017 Philippe Tillet
* 
* Permission is hereby granted, free of charge, to any person obtaining 
* a copy of this software and associated documentation files 
* (the "Software"), to deal in the Software without restriction, 
* including without limitation the rights to use, copy, modify, merge, 
* publish, distribute, sublicense, and/or sell copies of the Software, 
* and to permit persons to whom the Software is furnished to do so, 
* subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be 
* included in all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef ISAAC_MAPPED_OBJECT_H
#define ISAAC_MAPPED_OBJECT_H

#include <set>
#include <map>
#include <string>
#include "isaac/jit/syntax/engine/macro.h"
#include "isaac/jit/syntax/expression/expression.h"
#include "isaac/jit/generation/engine/stream.h"
#include "isaac/types.h"

namespace isaac
{

namespace symbolic
{

class object;

typedef std::map<size_t, std::shared_ptr<object> > symbols_table;

//Node
class node
{
public:
  node(size_t root, op_element op, expression_tree const & tree, symbols_table const & table);
  op_element op() const;
  object const * lhs() const;
  object const * rhs() const;
  size_t root() const;
protected:
  op_element op_;
  object* lhs_;
  object* rhs_;
  size_t root_;
};

//Object
class object
{
protected:
  void add_base(std::string const & name);
  void add_load(bool contiguous);
public:
  object(driver::Context const & context, std::string const & scalartype, unsigned int id);
  object(driver::Context const & context, std::string const & scalartype, std::string const & name);
  virtual ~object();
  bool hasattr(std::string const & name) const;
  std::string process(std::string const & in) const;
  virtual std::string evaluate(std::map<std::string, std::string> const & table) const;
protected:
  driver::Context const & context_;
  std::map<std::string, std::string> attributes_;
  std::set<macro> macros_;
  std::list<std::string> hierarchy_;
};

//Leaf
class leaf: public object
{
public:
  leaf(driver::Context const & context, std::string const & scalartype, unsigned int id);
  leaf(driver::Context const & context, std::string const & scalartype, std::string const & name);
};


//Arithmetic node
class arithmetic_node : public object, public node
{
public:
  arithmetic_node(unsigned int id, size_t root, op_element op, expression_tree const & tree, symbols_table const & table);
protected:
  std::string op_str_;
};

//Binary arithmetic
class binary_arithmetic_node: public arithmetic_node
{
public:
  binary_arithmetic_node(unsigned int id, size_t root, op_element op, expression_tree const & tree, symbols_table const & table);
  std::string evaluate(std::map<std::string, std::string> const & table) const;
};

//Unary arithmetic
class unary_arithmetic_node: public arithmetic_node
{
public:
  unary_arithmetic_node(unsigned int id, size_t root, op_element op, expression_tree const & tree, symbols_table const & table);
  std::string evaluate(std::map<std::string, std::string> const & table) const;
};

//Sfor
class sfor: public object, public node
{
public:
  sfor(unsigned int id, size_t root, op_element op, expression_tree const & tree, symbols_table const & table);
};

//Reductions
class reduction : public object, public node
{
public:
  reduction(unsigned int id, size_t root, op_element op, expression_tree const & tree, symbols_table const & table);
};

class reduce_1d : public reduction
{
public:
  reduce_1d(unsigned int id, size_t root, op_element op, expression_tree const & tree, symbols_table const & table);
};

class reduce_2d : public reduction
{
public:
  reduce_2d(unsigned int id, size_t root, op_element op, expression_tree const & tree, symbols_table const & table);
};

//Host scalar
class host_scalar : public leaf
{
public:
  host_scalar(driver::Context const & context, std::string const & scalartype, unsigned int id);
};

//Placeholder
class placeholder : public leaf
{
public:
  placeholder(driver::Context const & context, unsigned int level);
};

//Arrays
class array : public leaf
{
protected:
  std::string make_broadcast(tuple const & shape);
public:
  array(driver::Context const & context, std::string const & scalartype, unsigned int id);
};

//Buffer
class buffer : public array
{
public:
  buffer(driver::Context const & context, std::string const & scalartype, unsigned int id, tuple const & shape, tuple const &strides);
  unsigned int dim() const { return dim_; }
private:
  std::string ld_;
  std::string start_;
  std::string stride_;
  unsigned int dim_;
};

//Index modifier
class index_modifier: public array, public node
{
public:
  index_modifier(std::string const & scalartype, unsigned int id, size_t root, op_element op, expression_tree const & tree, symbols_table const & table);
};

class reshape : public index_modifier
{
public:
  reshape(std::string const & scalartype, unsigned int id, size_t root, op_element op, expression_tree const & tree, symbols_table const & table);
};

class trans : public index_modifier
{
public:
  trans(std::string const & scalartype, unsigned int id, size_t root, op_element op, expression_tree const & tree, symbols_table const & table);
};

class diag_vector : public index_modifier
{
public:
  diag_vector(std::string const & scalartype, unsigned int id, size_t root, op_element op, expression_tree const & tree, symbols_table const & table);
};




}

}
#endif
