#ifndef DTAI_DELTA_TREE_H
#define DTAI_DELTA_TREE_H

/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009 Alion Science and Technology
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Bradley Anderegg
 *
 */

#include <dtUtil/typetraits.h>
#include <cstddef>

#include <dtCore/refptr.h>
#include <dtCore/observerptr.h>
#include <dtUtil/referencedinterface.h>

namespace dtUtil
{

   template<class _Node>
   class TreeIteratorBase
   {
   public:
      typedef _Node value_type;
      typedef const typename dtUtil::TypeTraits<value_type>::pointer_type const_pointer;

      TreeIteratorBase(): _ptr(0), _root(0){}
      ~TreeIteratorBase(){ _ptr = 0; _root = 0;} //note: although this should be virtual, we have nothing to delete
      // so as an optimization I am going to ignore the virtual

      TreeIteratorBase(value_type* ptr, value_type* root): _ptr(ptr), _root(root){}

      TreeIteratorBase(const TreeIteratorBase& pIter): _ptr(pIter._ptr), _root(pIter._root){}

      TreeIteratorBase& operator=(const TreeIteratorBase& pIter)
      {
         _ptr= pIter._ptr;
         _root = pIter._root;
         return *this;
      }

      value_type* get() const{return _ptr;}
      bool valid() const {return _ptr != NULL;}

      value_type* _ptr;
      value_type* _root;
   };

   template<class _Node>
   class TreeIteratorChildBase
   {
   public:
      typedef _Node value_type;
      typedef const typename dtUtil::TypeTraits<value_type>::pointer_type const_pointer;

      TreeIteratorChildBase(): _ptr(0), _root(0){}
      ~TreeIteratorChildBase(){ _ptr = 0; _root = 0;} //note: although this should be virtual, we have nothing to delete
      // so as an optimization I am going to ignore the virtual

      TreeIteratorChildBase(value_type* ptr, value_type* root): _ptr(ptr), _root(root){}

      TreeIteratorChildBase(const TreeIteratorChildBase& pIter): _ptr(pIter._ptr), _root(pIter._root){}

      TreeIteratorChildBase& operator=(const TreeIteratorChildBase& pIter)
      {
         _ptr= pIter._ptr;
         _root = pIter._root;
         return *this;
      }

      value_type* get() const{return _ptr;}
      bool valid() const {return _ptr != NULL;}

      value_type* _ptr;
      value_type* _root;
   };

   template<class _Node>
   class TreeIteratorBase_Const
   {
   public:
      typedef _Node value_type;
      // typedef const typename dtUtil::TypeTraits<value_type>::pointer_type const_pointer;
      typedef const _Node* const_pointer;
      typedef typename dtUtil::TypeTraits<value_type>::const_reference const_reference;

      TreeIteratorBase_Const(): _ptr(0), _root(0){}
      ~TreeIteratorBase_Const(){ _ptr = 0; _root = 0;} //note: although this should be virtual, we have nothing to delete
      // so as an optimization I am going to ignore the virtual

      TreeIteratorBase_Const(const_pointer ptr, const_pointer root): _ptr(ptr), _root(root){}

      TreeIteratorBase_Const(const TreeIteratorBase_Const& pIter): _ptr(pIter._ptr), _root(pIter._root){}

      TreeIteratorBase_Const& operator=(const TreeIteratorBase_Const& pIter)
      {
         _ptr= pIter._ptr;
         _root = pIter._root;
         return *this;
      }

      TreeIteratorBase_Const(const TreeIteratorBase<_Node>& pIter): _ptr(pIter._ptr), _root(pIter._root){}

      TreeIteratorBase_Const& operator=(TreeIteratorBase<_Node>& pIter)
      {
         _ptr= pIter._ptr;
         _root = pIter._root;
         return *this;
      }

      const_pointer get() const{return _ptr;}
      bool valid() const {return _ptr != NULL;}

      const_pointer _ptr;
      const_pointer _root;
   };

   template<class _Node>
   class TreeIteratorChildBase_Const
   {
   public:
      typedef _Node value_type;
      //typedef const typename dtUtil::TypeTraits<value_type>::pointer_type const_pointer;
      typedef const _Node* const_pointer;
      typedef typename dtUtil::TypeTraits<value_type>::const_reference const_reference;

      TreeIteratorChildBase_Const(): _ptr(0), _root(0){}
      ~TreeIteratorChildBase_Const(){ _ptr = 0; _root = 0;} //note: although this should be virtual, we have nothing to delete
      // so as an optimization I am going to ignore the virtual

      TreeIteratorChildBase_Const(const_pointer ptr, const_pointer root): _ptr(ptr), _root(root){}

      TreeIteratorChildBase_Const(const TreeIteratorChildBase_Const& pIter): _ptr(pIter._ptr), _root(pIter._root){}

      TreeIteratorChildBase_Const& operator=(const TreeIteratorChildBase_Const& pIter)
      {
         _ptr= pIter._ptr;
         _root = pIter._root;
         return *this;
      }

      TreeIteratorChildBase_Const(const TreeIteratorChildBase<_Node>& pIter): _ptr(pIter._ptr), _root(pIter._root){}

      TreeIteratorChildBase_Const& operator=(TreeIteratorChildBase<_Node>& pIter)
      {
         _ptr= pIter._ptr;
         _root = pIter._root;
         return *this;
      }

      const_pointer get() const{return _ptr;}
      bool valid() const {return _ptr != NULL;}

      const_pointer _ptr;
      const_pointer _root;
   };

   // -----------------------------------------------------------------------------
   // Reverse Tree Iterator
   // -----------------------------------------------------------------------------

   template<class _Node>
   class ReverseTreeIterator: public TreeIteratorBase<_Node>
   {
   public:
      typedef TreeIteratorBase<_Node> BaseClass;
      typedef ReverseTreeIterator<_Node> MyType;
      typedef _Node value_type;

      ReverseTreeIterator(){}

      ReverseTreeIterator(const BaseClass& pIter): BaseClass(pIter){}

      ReverseTreeIterator(value_type* ptr, value_type* root): BaseClass(ptr, root){}

      ReverseTreeIterator& operator=(const BaseClass& pIter)
      {
         return BaseClass(pIter);
      }

      ~ReverseTreeIterator(){ BaseClass::_ptr = 0; BaseClass::_root = 0;}

      _Node& operator*() const{ return *BaseClass::_ptr; }

      _Node* operator->() const{return BaseClass::_ptr; }

      bool operator==(const ReverseTreeIterator& pIter) const{ return BaseClass::_ptr == pIter._ptr; }

      bool operator!=(const ReverseTreeIterator& pIter) const{ return !(BaseClass::_ptr == pIter._ptr); }

      ReverseTreeIterator& operator--()
      {
         BaseClass::_ptr = BaseClass::_ptr->next();
         return *this;
      }

      ReverseTreeIterator& operator++()
      {
         if (BaseClass::_ptr) BaseClass::_ptr = BaseClass::_ptr->prev();
         else BaseClass::_ptr = BaseClass::_root->last_descendant();
         return *this;
      }
   };

   // -----------------------------------------------------------------------------
   // Forward Tree Child Iterator
   // -----------------------------------------------------------------------------
   template<class _Node>
   class ForwardTreeChildIterator: public TreeIteratorChildBase<_Node>
   {
   public:
      typedef TreeIteratorChildBase<_Node> BaseClass;
      typedef ForwardTreeChildIterator<_Node> MyType;
      typedef _Node value_type;

      ForwardTreeChildIterator(){}

      ForwardTreeChildIterator(const BaseClass& pIter): BaseClass(pIter){}

      ForwardTreeChildIterator(value_type* ptr, value_type* root): BaseClass(ptr, root) {}

      ForwardTreeChildIterator& operator=(const BaseClass& pIter)
      {
         return BaseClass(pIter);
      }

      _Node& operator*() const{ return *BaseClass::_ptr; }

      _Node* operator->() const{return BaseClass::_ptr; }

      bool operator==(const ForwardTreeChildIterator& pIter) const{ return BaseClass::_ptr == pIter._ptr; }

      bool operator!=(const ForwardTreeChildIterator& pIter) const{ return !(BaseClass::_ptr == pIter._ptr); }

      ForwardTreeChildIterator& operator++()
      {
         BaseClass::_ptr = BaseClass::_ptr->next_sibling();
         return *this;
      }

      ForwardTreeChildIterator& operator--()
      {
         if(BaseClass::_ptr) BaseClass::_ptr = BaseClass::_ptr->prev_sibling();
         else BaseClass::_ptr = BaseClass::_root->last_child();
         return *this;
      }
   };

   // -----------------------------------------------------------------------------
   // Reverse Tree Iterator
   // -----------------------------------------------------------------------------

   template<class _Node>
   class ReverseTreeChildIterator: public TreeIteratorChildBase<_Node>
   {
   public:
      typedef TreeIteratorChildBase<_Node> BaseClass;
      typedef ReverseTreeChildIterator<_Node> MyType;
      typedef _Node value_type;

      ReverseTreeChildIterator(){}

      ReverseTreeChildIterator(const BaseClass& pIter): BaseClass(pIter){}

      ReverseTreeChildIterator(value_type* ptr, value_type* root): BaseClass(ptr, root){}

      ReverseTreeChildIterator& operator=(const BaseClass& pIter)
      {
         return BaseClass(pIter);
      }

      _Node& operator*() const{ return *BaseClass::_ptr; }

      _Node* operator->() const{return BaseClass::_ptr; }

      bool operator==(const ReverseTreeChildIterator& pIter) const{ return BaseClass::_ptr == pIter._ptr; }

      bool operator!=(const ReverseTreeChildIterator& pIter) const{ return !(BaseClass::_ptr == pIter._ptr); }

      ReverseTreeChildIterator& operator--()
      {
         BaseClass::_ptr = BaseClass::_ptr->next_sibling();
         return *this;
      }

      ReverseTreeChildIterator& operator++()
      {
         if(BaseClass::_ptr) BaseClass::_ptr = BaseClass::_ptr->prev_sibling();
         else BaseClass::_ptr = BaseClass::_root->last_child();
         return *this;
      }
   };

   // -----------------------------------------------------------------------------
   // Forward Tree Iterator
   // -----------------------------------------------------------------------------
   template<class _Node>
   class ForwardTreeIterator: public TreeIteratorBase<_Node>
   {
   public:
      typedef TreeIteratorBase<_Node> BaseClass;
      typedef ForwardTreeIterator<_Node> MyType;
      typedef _Node value_type;

      ForwardTreeIterator(){}

      ForwardTreeIterator(const BaseClass& pIter): BaseClass(pIter){}

      ForwardTreeIterator(value_type* ptr, value_type* root): BaseClass(ptr, root){}

      ForwardTreeIterator& operator=(const BaseClass& pIter)
      {
         return BaseClass(pIter);
      }

      _Node& operator*() const{ return *BaseClass::_ptr; }

      _Node* operator->() const{return BaseClass::_ptr; }

      bool operator==(const ForwardTreeIterator& pIter) const{ return BaseClass::_ptr == pIter._ptr; }

      bool operator!=(const ForwardTreeIterator& pIter) const{ return !(BaseClass::_ptr == pIter._ptr); }

      ForwardTreeIterator& operator++()
      {
         BaseClass::_ptr = BaseClass::_ptr->next();
         return *this;
      }

      ForwardTreeIterator& operator--()
      {
         if(BaseClass::_ptr) BaseClass::_ptr = BaseClass::_ptr->prev();
         else BaseClass::_ptr = BaseClass::_root->last_descendant();
         return *this;
      }
   };

   // -----------------------------------------------------------------------------
   // Const Tree Iterator
   // -----------------------------------------------------------------------------

   template<class _Node>
   class ForwardTreeIterator_Const: public TreeIteratorBase_Const<_Node>
   {
   public:
      typedef TreeIteratorBase_Const<_Node> BaseClass;
      typedef ForwardTreeIterator_Const<_Node> MyType;
      typedef _Node value_type;
      typedef const _Node* const_pointer;

      ForwardTreeIterator_Const(): BaseClass(){}

      ForwardTreeIterator_Const(const BaseClass& pIter): BaseClass(pIter){}
      ForwardTreeIterator_Const(const TreeIteratorBase<_Node>& pIter): BaseClass(pIter){}

      ForwardTreeIterator_Const(const_pointer ptr, const_pointer root): BaseClass(ptr, root){}

      ForwardTreeIterator_Const& operator=(const ForwardTreeIterator_Const& pIter)
      {
         return BaseClass::operator=(pIter);
      }

      ForwardTreeIterator_Const& operator=(const ForwardTreeIterator<_Node>& pIter)
      {
         return BaseClass::operator=(pIter);
      }

      const _Node& operator*() const{ return *BaseClass::_ptr; }
      const _Node* operator->() const{return BaseClass::_ptr;}

      bool operator==(const ForwardTreeIterator_Const& pIter) const{ return BaseClass::_ptr == pIter._ptr; }

      bool operator!=(const ForwardTreeIterator_Const& pIter) const{ return !(BaseClass::_ptr == pIter._ptr); }

      ForwardTreeIterator_Const& operator++()
      {
         BaseClass::_ptr = BaseClass::_ptr->next();
         return *this;
      }

      ForwardTreeIterator_Const& operator--()
      {
         if(BaseClass::_ptr) BaseClass::_ptr = BaseClass::_ptr->prev();
         else BaseClass::_ptr = BaseClass::_root->last_descendant();
         return *this;
      }
   };

   // -----------------------------------------------------------------------------
   // Const Reverse Tree Iterator
   // -----------------------------------------------------------------------------

   template<class _Node>
   class ReverseTreeIterator_Const: public TreeIteratorBase_Const<_Node>
   {
   public:
      typedef TreeIteratorBase_Const<_Node> BaseClass;
      typedef ReverseTreeIterator_Const<_Node> MyType;
      typedef _Node value_type;
      typedef const _Node* const_pointer;

      ReverseTreeIterator_Const(): BaseClass(){}

      ReverseTreeIterator_Const(const BaseClass& pIter): BaseClass(pIter){}
      ReverseTreeIterator_Const(TreeIteratorBase<_Node>& pIter): BaseClass(pIter){}

      ReverseTreeIterator_Const(const_pointer ptr, const_pointer root): BaseClass(ptr, root){}

      ReverseTreeIterator_Const& operator=(const ReverseTreeIterator_Const& pIter)
      {
         return BaseClass::operator=(pIter);
      }

      ReverseTreeIterator_Const& operator=(const ForwardTreeIterator<_Node>& pIter)
      {
         return BaseClass::operator=(pIter);
      }

      ReverseTreeIterator_Const& operator=(const ReverseTreeIterator<_Node>& pIter)
      {
         BaseClass::_ptr= pIter._ptr;
         BaseClass::_root = pIter._root;
         return *this;
      }

      const _Node& operator*() const{ return *BaseClass::_ptr; }
      const _Node* operator->() const{return BaseClass::_ptr;}

      bool operator==(const ReverseTreeIterator_Const& pIter) const{ return BaseClass::_ptr == pIter._ptr; }

      bool operator!=(const ReverseTreeIterator_Const& pIter) const{ return !(BaseClass::_ptr == pIter._ptr); }

      ReverseTreeIterator_Const& operator--()
      {
         BaseClass::_ptr = BaseClass::_ptr->next();
         return *this;
      }

      ReverseTreeIterator_Const& operator++()
      {
         if(BaseClass::_ptr) BaseClass::_ptr = BaseClass::_ptr->prev();
         else BaseClass::_ptr = BaseClass::_root->last_descendant();
         return *this;
      }
   };

   // -----------------------------------------------------------------------------
   // Const Tree Iterator
   // -----------------------------------------------------------------------------

   template<class _Node>
   class ForwardTreeChildIterator_Const: public TreeIteratorChildBase_Const<_Node>
   {
   public:
      typedef ForwardTreeChildIterator_Const<_Node> MyType;
      typedef TreeIteratorChildBase_Const<_Node> BaseClass;
      typedef _Node value_type;
      typedef const _Node* const_pointer;

      ForwardTreeChildIterator_Const(): BaseClass(){}

      ForwardTreeChildIterator_Const(const BaseClass& pIter): BaseClass(pIter){}
      ForwardTreeChildIterator_Const(const TreeIteratorChildBase<_Node>& pIter): BaseClass(pIter){}

      ForwardTreeChildIterator_Const(const_pointer ptr, const_pointer root): BaseClass(ptr, root){}

      ForwardTreeChildIterator_Const& operator=(const ForwardTreeChildIterator_Const& pIter)
      {
         return BaseClass::operator=(pIter);
      }

      ForwardTreeChildIterator_Const& operator=(const ForwardTreeIterator<_Node>& pIter)
      {
         return BaseClass::operator=(pIter);
      }

      ForwardTreeChildIterator_Const& operator=(const ReverseTreeIterator<_Node>& pIter)
      {
         BaseClass::_ptr= pIter._ptr;
         BaseClass::_root = pIter._root;
         return *this;
      }

      const _Node& operator*() const{ return *BaseClass::_ptr; }

      const _Node* operator->() const{return BaseClass::_ptr;}

      bool operator==(const ForwardTreeChildIterator_Const& pIter) const{ return BaseClass::_ptr == pIter._ptr; }

      bool operator!=(const ForwardTreeChildIterator_Const& pIter) const{ return !(BaseClass::_ptr == pIter._ptr); }

      ForwardTreeChildIterator_Const& operator++()
      {
         BaseClass::_ptr = BaseClass::_ptr->next_sibling();
         return *this;
      }

      ForwardTreeChildIterator_Const& operator--()
      {
         if(BaseClass::_ptr) BaseClass::_ptr = BaseClass::_ptr->prev_sibling();
         else BaseClass::_ptr = BaseClass::_root->last_child();
         return *this;
      }
   };

   // -----------------------------------------------------------------------------
   // Const Reverse Tree Iterator
   // -----------------------------------------------------------------------------

   template<class _Node>
   class ReverseTreeChildIterator_Const: public TreeIteratorChildBase_Const<_Node>
   {
   public:
      typedef ReverseTreeChildIterator_Const<_Node> MyType;
      typedef TreeIteratorChildBase_Const<_Node> BaseClass;
      typedef const _Node* const_pointer;

      ReverseTreeChildIterator_Const(): BaseClass(){}

      ReverseTreeChildIterator_Const(const BaseClass& pIter): BaseClass(pIter){}
      ReverseTreeChildIterator_Const(const TreeIteratorChildBase<_Node>& pIter): BaseClass(pIter){}

      ReverseTreeChildIterator_Const(const_pointer ptr, const_pointer root): BaseClass(ptr, root){}

      ReverseTreeChildIterator_Const& operator=(const ReverseTreeChildIterator_Const& pIter)
      {
         return BaseClass::operator=(pIter);
      }

      ReverseTreeChildIterator_Const& operator=(const ForwardTreeIterator<_Node>& pIter)
      {
         return BaseClass::operator=(pIter);
      }

      ReverseTreeChildIterator_Const& operator=(const ReverseTreeIterator<_Node>& pIter)
      {
         BaseClass::_ptr= pIter._ptr;
         BaseClass::_root = pIter._root;
         return *this;
      }

      _Node& operator*() const{ return *BaseClass::_ptr; }
      const _Node* operator->() const{return BaseClass::_ptr;}

      bool operator==(const ReverseTreeChildIterator_Const& pIter) const{ return BaseClass::_ptr == pIter._ptr; }

      bool operator!=(const ReverseTreeChildIterator_Const& pIter) const{ return !(BaseClass::_ptr == pIter._ptr); }

      ReverseTreeChildIterator_Const& operator--()
      {
         BaseClass::_ptr = BaseClass::_ptr->next_sibling();
         return *this;
      }

      ReverseTreeChildIterator_Const& operator++()
      {
         if(BaseClass::_ptr) BaseClass::_ptr = BaseClass::_ptr->prev_sibling();
         else BaseClass::_ptr = BaseClass::_root->last_child();
         return *this;
      }
   };

   template <class T, class T_BaseClass = osg::Referenced>
   class Tree : public T_BaseClass
   {
      ////////////////////////////////////////////////////////////////////////////////////////////
      //Data Structures
      ////////////////////////////////////////////////////////////////////////////////////////////
   public:
      typedef T_BaseClass BaseClass;

      typedef Tree<T, T_BaseClass> MyType;
      typedef MyType TreeNode;

      typedef ForwardTreeIterator<TreeNode> iterator;
      typedef ForwardTreeIterator_Const<TreeNode> const_iterator;
      typedef ReverseTreeIterator<TreeNode> reverse_iterator;
      typedef ReverseTreeIterator_Const<TreeNode> const_reverse_iterator;

      typedef ForwardTreeChildIterator<TreeNode> child_iterator;
      typedef ForwardTreeChildIterator_Const<TreeNode> const_child_iterator;
      typedef ReverseTreeChildIterator<TreeNode> reverse_child_iterator;
      typedef ReverseTreeChildIterator_Const<TreeNode> const_reverse_child_iterator;

      typedef typename dtUtil::TypeTraits<T>::value_type value_type;
      typedef typename dtUtil::TypeTraits<T>::const_reference vt_const_ref;
      typedef typename dtUtil::TypeTraits<T>::pointer_type vt_ptr;

      typedef MyType& reference;
      typedef const MyType& const_reference;
      typedef MyType* pointer;
      typedef const MyType* const_pointer;
      typedef dtCore::RefPtr<MyType> ref_pointer;

      /////////////////////////////////////////////////////////////////////////////////////////////
      //Functions
      /////////////////////////////////////////////////////////////////////////////////////////////
   public:
      Tree();
      Tree(T pData);
      Tree(const Tree&);
      Tree& operator=(const Tree&);

      // If this tree is derived, overload this function and return an instance of your derived class
      virtual ref_pointer clone() const { return NULL; }

   protected:
      virtual ~Tree();

   public:
      void clear();

      bool empty() const;

      //number of decendants
      unsigned size() const;

      //depth in the tree
      unsigned level() const;

      //number of children
      unsigned degree() const;

      bool is_leaf() const;
      bool is_root() const;
      bool is_descendant_of(const_reference) const;

      pointer prev() const;
      pointer next() const;
      pointer first_child() const;
      pointer last_child() const;
      pointer next_sibling() const;
      pointer prev_sibling() const;
      pointer last_descendant() const;
      pointer parent() const;

      reference front();
      const_reference front() const;

      reference back();
      const_reference back() const;

      void push_front(ref_pointer subtree);
      void pop_front();

      void push_back(ref_pointer subtree);
      void pop_back();

      iterator erase(iterator pWhere);
      reverse_iterator erase(reverse_iterator pWhere);
      child_iterator erase(child_iterator pWhere);
      reverse_child_iterator erase(reverse_child_iterator pWhere);

      iterator insert(ref_pointer subtree, iterator pWhere);
      reverse_iterator insert(ref_pointer subtree, reverse_iterator pWhere);
      child_iterator insert(ref_pointer subtree, child_iterator pWhere);
      reverse_child_iterator insert(ref_pointer subtree, reverse_child_iterator pWhere);

      /**
       * get an iterator performing a pre-order traversal
       */
      iterator begin();
      const_iterator begin() const;

      /**
       * get an iterator pointing to the end of the pre-order traversal
       */
      iterator end();
      const_iterator end() const;

      /**
       * iterate through all immediate children
       */
      child_iterator begin_child();
      const_child_iterator begin_child() const;

      /**
       * get an iterator pointing to the end of the immediate children
       */
      child_iterator end_child();
      const_child_iterator end_child() const;

      /**
       * reverse iterator performs a post-order traversal
       */
      reverse_iterator rbegin();
      const_reverse_iterator rbegin() const;

      /**
       * reverse iterator performs a post-order traversal
       */
      reverse_iterator rend();
      const_reverse_iterator rend() const;

      /**
       * reverse iterate through all immediate children
       */
      reverse_child_iterator rbegin_child();
      const_reverse_child_iterator rbegin_child() const;

      /**
       * reverse iterate through all immediate children
       */
      reverse_child_iterator rend_child();
      const_reverse_child_iterator rend_child() const;

      T value;

      void remove_subtree(pointer tree);
      void insert_subtree(pointer tree, pointer next);

   private:
      void init();
      void destroy_decendants();
      void change_last_descendant(pointer tree);

      void copy_children(const_pointer tree);

      pointer mParent;
      pointer mPrevSibling;
      ref_pointer mNext;
      pointer mLastDescendant;
   };

   template <class T, class T_BaseClass>
   Tree<T, T_BaseClass>::Tree()
      : value(NULL)
      , mParent(NULL)
      , mPrevSibling(NULL)
      , mLastDescendant(NULL)
   {
      init();
   }

   template <class T, class T_BaseClass>
   Tree<T, T_BaseClass>::Tree(T pData)
      : value(pData)
      , mParent(NULL)
      , mPrevSibling(NULL)
      , mLastDescendant(NULL)
   {
      init();
   }

   template <class T, class T_BaseClass>
   Tree<T, T_BaseClass>::Tree(const Tree<T, T_BaseClass>& pTree)
   {
      init();
      copy_children(&pTree);
   }

   template <class T, class T_BaseClass>
   Tree<T, T_BaseClass>& Tree<T, T_BaseClass>::operator=(const Tree<T, T_BaseClass>& pTree)
   {
      clear();
      init();
      value = pTree.value;
      copy_children(&pTree);
      return *this;
   }

   template <class T, class T_BaseClass>
   Tree<T, T_BaseClass>::~Tree()
   {
      mParent = NULL;
      mPrevSibling = NULL;
      mNext = NULL;
      mLastDescendant = NULL;
      value = NULL;
   }

   template <class T, class T_BaseClass>
   void Tree<T, T_BaseClass>::init()
   {
      mParent = mNext = 0;
      mPrevSibling = mLastDescendant = this;
   }

   template <class T, class T_BaseClass>
   void Tree<T, T_BaseClass>::clear()
   {
      destroy_decendants();

      mParent = NULL;
      mPrevSibling = NULL;
      mNext = NULL;
      mLastDescendant = NULL;
   }

   template <class T, class T_BaseClass>
   bool Tree<T, T_BaseClass>::empty() const
   {
      return degree() == 0;
   }


   template <class T, class T_BaseClass>
   unsigned Tree<T, T_BaseClass>::size() const
   {
      unsigned n = 1;
      for (const TreeNode* t = this; t != mLastDescendant; t = t->next())
      {
         ++n;
      }

      return n;
   }

   template <class T, class T_BaseClass>
   unsigned Tree<T, T_BaseClass>::level() const
   {
      unsigned l = 0;
      for (const TreeNode* t = mParent; t != NULL; t = t->parent())
      {
         ++l;
      }

      return l;
   }

   template <class T, class T_BaseClass>
   unsigned Tree<T, T_BaseClass>::degree() const
   {
      unsigned n = 0;
      for (const TreeNode* t = first_child(); t; t = t->next_sibling())
      {
         ++n;
      }
      return n;
   }

   template <class T, class T_BaseClass>
   bool Tree<T, T_BaseClass>::is_leaf() const
   {
      return mLastDescendant == this;
   }

   template <class T, class T_BaseClass>
   bool Tree<T, T_BaseClass>::is_root() const
   {
      return mParent == NULL;
   }

   template <class T, class T_BaseClass>
   bool Tree<T, T_BaseClass>::is_descendant_of(const_reference ancestor) const
   {
      const TreeNode* t = this;
      for (; t; t = t->parent())
      {
         if (t == &ancestor)
         {
            break;
         }
      }
      return (t != 0) && t != this;
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::pointer Tree<T, T_BaseClass>::prev() const
   {
      TreeNode* prev = 0;
      if(mParent != NULL)
      {
         if(mParent->next() == this)
         {
            prev = mParent;
         }
         else
         {
            prev = mPrevSibling->mLastDescendant;
         }
      }

      return prev;
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::pointer Tree<T, T_BaseClass>::next() const
   {
      return mNext.get();
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::pointer Tree<T, T_BaseClass>::parent() const
   {
      return mParent;
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::pointer Tree<T, T_BaseClass>::first_child() const
   {
      TreeNode* child = 0;
      if(mNext.valid() && (mNext->mParent == this))
      {
         child = next();
      }
      return child;
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::pointer Tree<T, T_BaseClass>::last_child() const
   {
      TreeNode* lastChild = first_child();
      if(lastChild)
      {
         lastChild = lastChild->mPrevSibling;
      }
      return lastChild;
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::pointer Tree<T, T_BaseClass>::next_sibling() const
   {
      TreeNode* nextSibling = mLastDescendant->next();
      if(nextSibling && nextSibling->mParent != mParent)
      {
         nextSibling = 0;
      }
      return nextSibling;
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::pointer Tree<T, T_BaseClass>::prev_sibling() const
   {
      TreeNode* prevSibling = 0;
      if(mParent != NULL && (mParent->next() != this))
      {
         prevSibling = mPrevSibling;
      }
      return prevSibling;
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::pointer Tree<T, T_BaseClass>::last_descendant() const
   {
      return mLastDescendant;
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::reference Tree<T, T_BaseClass>::front()
   {
      return *first_child();
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::reference Tree<T, T_BaseClass>::back()
   {
      return *last_child();
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::const_reference Tree<T, T_BaseClass>::front() const
   {
      return *first_child();
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::const_reference Tree<T, T_BaseClass>::back() const
   {
      return *last_child();
   }

   template <class T, class T_BaseClass>
   void Tree<T, T_BaseClass>::pop_front()
   {
      ref_pointer first = first_child();
      erase(iterator(first.get(), this));
   }

   template <class T, class T_BaseClass>
   void Tree<T, T_BaseClass>::pop_back()
   {
      ref_pointer last = last_child();
      erase(iterator(last.get(), this));
   }

   template <class T, class T_BaseClass>
   void Tree<T, T_BaseClass>::push_front(ref_pointer subtree)
   {
      ref_pointer next = first_child();
      insert_subtree(subtree.get(), next);
   }

   template <class T, class T_BaseClass>
   void Tree<T, T_BaseClass>::push_back(ref_pointer subtree)
   {
      insert_subtree(subtree.get(), 0);
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::iterator Tree<T, T_BaseClass>::erase(iterator pWhere)
   {
      ref_pointer child = &*pWhere;
      ref_pointer parent = child->mParent;
      ref_pointer next = child->next_sibling();
      parent->remove_subtree(child.get());
      child->clear();

      return iterator(next.get(), this);
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::reverse_iterator Tree<T, T_BaseClass>::erase(reverse_iterator pWhere)
   {
      ref_pointer child = &*pWhere;
      ref_pointer parent = child->mParent;
      ref_pointer rev_next = child->prev_sibling();
      parent->remove_subtree(child.get());
      child->clear();

      return reverse_iterator(rev_next.get(), this);
   }


   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::child_iterator Tree<T, T_BaseClass>::erase(child_iterator pWhere)
   {
      ref_pointer child = &*pWhere;
      ref_pointer parent = child->mParent;
      ref_pointer next = child->next_sibling();
      parent->remove_subtree(child.get());
      child->clear();

      return child_iterator(next.get(), this);
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::reverse_child_iterator Tree<T, T_BaseClass>::erase(reverse_child_iterator pWhere)
   {
      ref_pointer child = &*pWhere;
      ref_pointer parent = child->mParent;
      ref_pointer rev_next = child->prev_sibling();
      parent->remove_subtree(child.get());
      child->clear();

      return reverse_child_iterator(rev_next.get(), this);
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::iterator Tree<T, T_BaseClass>::insert(ref_pointer subtree, iterator pIter)
   {
      insert_subtree(subtree.get(), &*pIter);   // if end(), append to this
      return iterator(subtree.get(), this);
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::reverse_iterator Tree<T, T_BaseClass>::insert(ref_pointer subtree, reverse_iterator pIter)
   {
      insert_subtree(subtree.get(), &*pIter);   // if end(), append to this
      return reverse_iterator(subtree.get(), this);
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::child_iterator Tree<T, T_BaseClass>::insert(ref_pointer subtree, child_iterator pIter)
   {
      insert_subtree(subtree.get(), &*pIter);   // if end(), append to this
      return child_iterator(subtree.get(), this);
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::reverse_child_iterator Tree<T, T_BaseClass>::insert(ref_pointer subtree, reverse_child_iterator pIter)
   {
      insert_subtree(subtree.get(), &*pIter);   // if end(), append to this
      return reverse_child_iterator(subtree.get(), this);
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::iterator Tree<T, T_BaseClass>::begin()
   {
      return iterator(this, this);
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::const_iterator Tree<T, T_BaseClass>::begin() const
   {
      return const_iterator(this, this);
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::child_iterator Tree<T, T_BaseClass>::begin_child()
   {
      return typename Tree<T, T_BaseClass>::child_iterator(first_child(), this);
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::const_child_iterator Tree<T, T_BaseClass>::begin_child() const
   {
      return typename Tree<T, T_BaseClass>::const_child_iterator(first_child(), this);
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::reverse_iterator Tree<T, T_BaseClass>::rbegin()
   {
      return reverse_iterator(last_descendant(), this);
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::const_reverse_iterator Tree<T, T_BaseClass>::rbegin() const
   {
      return const_reverse_iterator(last_descendant(), this);
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::reverse_child_iterator Tree<T, T_BaseClass>::rbegin_child()
   {
      return reverse_child_iterator(last_child(), this);
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::const_reverse_child_iterator Tree<T, T_BaseClass>::rbegin_child() const
   {
      return const_reverse_child_iterator(last_child(), this);
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::iterator Tree<T, T_BaseClass>::end()
   {
      return iterator(0, this);
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::const_iterator Tree<T, T_BaseClass>::end() const
   {
      return const_iterator(0, this);
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::child_iterator Tree<T, T_BaseClass>::end_child()
   {
      return typename Tree<T, T_BaseClass>::child_iterator(0, this);
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::const_child_iterator Tree<T, T_BaseClass>::end_child() const
   {
      return typename Tree<T, T_BaseClass>::const_child_iterator(0, this);
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::reverse_iterator Tree<T, T_BaseClass>::rend()
   {
      return reverse_iterator(0, this);
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::const_reverse_iterator Tree<T, T_BaseClass>::rend() const
   {
      return const_reverse_iterator(0, this);
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::reverse_child_iterator Tree<T, T_BaseClass>::rend_child()
   {
      return reverse_child_iterator(0, this);
   }

   template <class T, class T_BaseClass>
   typename Tree<T, T_BaseClass>::const_reverse_child_iterator Tree<T, T_BaseClass>::rend_child() const
   {
      return const_reverse_child_iterator(0, this);
   }

   template <class T, class T_BaseClass>
   void Tree<T, T_BaseClass>::destroy_decendants()
   {
      mPrevSibling = mLastDescendant = this;
      if(mNext.valid() && mLastDescendant != this)
      {
         ref_pointer descendant = first_child();
         ref_pointer end = last_descendant()->next();
         while (descendant.valid())
         {
            pointer next = descendant->next_sibling();
            descendant->destroy_decendants();
            descendant = next;
         }

         mNext = end;
         mLastDescendant = this;
      }
   }

   template <class T, class T_BaseClass>
   void Tree<T, T_BaseClass>::copy_children(const_pointer pTree)
   {
      for (const TreeNode* child = pTree->first_child(); child != NULL; child = child->next_sibling())
      {
         insert_subtree(child->clone(), 0);
      }
   }

   template <class T, class T_BaseClass>
   void Tree<T, T_BaseClass>::change_last_descendant(pointer newLast)
   {
      // Go up to ancestors referencing the current last descendant
      // and change them to point to the new last descendant so the
      // whole tree remains in synch.
      pointer oldLast = mLastDescendant;

      // NOTE: the last descendant should not be NULL.
      pointer ancestor = this;

      do
      {
         ancestor->mLastDescendant = newLast;
         ancestor = ancestor->mParent;
      }
      while (ancestor != NULL && (ancestor->mLastDescendant == oldLast));
   }

   template <class T, class T_BaseClass>
   void Tree<T, T_BaseClass>::remove_subtree(pointer child)
   {
      pointer sib = child->next_sibling();
      if (sib != NULL)
      {
         sib->mPrevSibling = child->mPrevSibling;
      }
      else
      {
         first_child()->mPrevSibling = child->mPrevSibling;
      }

      // The Next pointer keeps children in existence.
      // Since Next is about to change, keep a temporary
      // pointer to ensure child instances exist until
      // all other operations in this method have had
      // their chances to complete.
      ref_pointer tmp = mNext;

      if (mLastDescendant == child->mLastDescendant)
      {
         change_last_descendant(child->prev());
      }

      if (mNext == child)   // deleting first child?
      {
         mNext = child->mLastDescendant->mNext;
      }
      else
      {
         child->mPrevSibling->mLastDescendant->mNext = child->mLastDescendant->mNext;
      }

      if (child->mParent == this)
      {
         child->mParent = NULL;
      }

      // If the last decendant of the removed subtree points to a
      // sibling in the current tree, make sure its next pointer is
      // nullified so that it does not hold onto the sibling inadvertantly.
      // The current tree's next should be the next sibling to the child being removed.
      if (child->mLastDescendant != NULL
         && child->mLastDescendant->mNext == mNext)
      {
         child->mLastDescendant->mNext = NULL;
      }

      // Cleanup.
      // The temporary pointer can now be released since everything
      // have completed up to this point.
      tmp = NULL;
   }

   template <class T, class T_BaseClass>
   void Tree<T, T_BaseClass>::insert_subtree(pointer pSubTree, pointer pNext)
   {
      if (pNext == NULL)
      {
         // append as last child
         pSubTree->mParent = this;
         pSubTree->mLastDescendant->mNext = mLastDescendant->mNext;
         mLastDescendant->mNext = pSubTree;

         pSubTree->mPrevSibling = last_child();
         if (is_leaf())
         {
            mNext = pSubTree;
         }

         first_child()->mPrevSibling = pSubTree;

         change_last_descendant(pSubTree->mLastDescendant);
      }
      else
      {
         ref_pointer parent = pNext->mParent;
         pSubTree->mParent = parent;
         pSubTree->mPrevSibling = pNext->mPrevSibling;

         pSubTree->mLastDescendant->mNext = pNext;
         if (parent->mNext == pNext)   // inserting before first subtree?
         {
            parent->mNext = pSubTree;
         }
         else
         {
            pNext->mPrevSibling->mLastDescendant->mNext = pSubTree;
         }

         pNext->mPrevSibling = pSubTree;
      }
   }

} // namespace dtAI

#endif //DTAI_DELTA_TREE_H
