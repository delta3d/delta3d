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

#include <osg/Referenced>
#include <dtCore/refptr.h>
#include <dtCore/observerptr.h>

namespace dtAI
{

   template<class _Node>
   class _TreeIteratorBase
   {
   public:
      typedef _Node value_type;
      typedef const typename dtUtil::TypeTraits<value_type>::pointer_type const_pointer;

      _TreeIteratorBase(): _ptr(0), _root(0){}
      ~_TreeIteratorBase(){ _ptr = 0; _root = 0;} //note: although this should be virtual, we have nothing to delete
      // so as an optimization I am going to ignore the virtual

      _TreeIteratorBase(value_type* ptr, value_type* root): _ptr(ptr), _root(root){}

      _TreeIteratorBase(const _TreeIteratorBase& pIter): _ptr(pIter._ptr), _root(pIter._root){}

      _TreeIteratorBase& operator=(const _TreeIteratorBase& pIter)
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
   class _TreeIteratorChildBase
   {
   public:
      typedef _Node value_type;
      typedef const typename dtUtil::TypeTraits<value_type>::pointer_type const_pointer;

      _TreeIteratorChildBase(): _ptr(0), _root(0){}
      ~_TreeIteratorChildBase(){ _ptr = 0; _root = 0;} //note: although this should be virtual, we have nothing to delete
      // so as an optimization I am going to ignore the virtual

      _TreeIteratorChildBase(value_type* ptr, value_type* root): _ptr(ptr), _root(root){}

      _TreeIteratorChildBase(const _TreeIteratorChildBase& pIter): _ptr(pIter._ptr), _root(pIter._root){}

      _TreeIteratorChildBase& operator=(const _TreeIteratorChildBase& pIter)
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
   class _TreeIteratorBase_Const
   {
   public:
      typedef _Node value_type;
      // typedef const typename dtUtil::TypeTraits<value_type>::pointer_type const_pointer;
      typedef const _Node* const_pointer;
      typedef typename dtUtil::TypeTraits<value_type>::const_reference const_reference;

      _TreeIteratorBase_Const(): _ptr(0), _root(0){}
      ~_TreeIteratorBase_Const(){ _ptr = 0; _root = 0;} //note: although this should be virtual, we have nothing to delete
      // so as an optimization I am going to ignore the virtual

      _TreeIteratorBase_Const(const_pointer ptr, const_pointer root): _ptr(ptr), _root(root){}

      _TreeIteratorBase_Const(const _TreeIteratorBase_Const& pIter): _ptr(pIter._ptr), _root(pIter._root){}

      _TreeIteratorBase_Const& operator=(const _TreeIteratorBase_Const& pIter)
      {
         _ptr= pIter._ptr;
         _root = pIter._root;
         return *this;
      }

      _TreeIteratorBase_Const(const _TreeIteratorBase<_Node>& pIter): _ptr(pIter._ptr), _root(pIter._root){}

      _TreeIteratorBase_Const& operator=(_TreeIteratorBase<_Node>& pIter)
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
   class _TreeIteratorChildBase_Const
   {
   public:
      typedef _Node value_type;
      //typedef const typename dtUtil::TypeTraits<value_type>::pointer_type const_pointer;
      typedef const _Node* const_pointer;
      typedef typename dtUtil::TypeTraits<value_type>::const_reference const_reference;

      _TreeIteratorChildBase_Const(): _ptr(0), _root(0){}
      ~_TreeIteratorChildBase_Const(){ _ptr = 0; _root = 0;} //note: although this should be virtual, we have nothing to delete
      // so as an optimization I am going to ignore the virtual

      _TreeIteratorChildBase_Const(const_pointer ptr, const_pointer root): _ptr(ptr), _root(root){}

      _TreeIteratorChildBase_Const(const _TreeIteratorChildBase_Const& pIter): _ptr(pIter._ptr), _root(pIter._root){}

      _TreeIteratorChildBase_Const& operator=(const _TreeIteratorChildBase_Const& pIter)
      {
         _ptr= pIter._ptr;
         _root = pIter._root;
         return *this;
      }

      _TreeIteratorChildBase_Const(const _TreeIteratorChildBase<_Node>& pIter): _ptr(pIter._ptr), _root(pIter._root){}

      _TreeIteratorChildBase_Const& operator=(_TreeIteratorChildBase<_Node>& pIter)
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
   class _RevTreeIterator: public _TreeIteratorBase<_Node>
   {
   public:
      typedef _TreeIteratorBase<_Node> BaseClass;
      typedef _RevTreeIterator<_Node> MyType;
      typedef _Node value_type;

      _RevTreeIterator(){}

      _RevTreeIterator(const BaseClass& pIter): BaseClass(pIter){}

      _RevTreeIterator(value_type* ptr, value_type* root): BaseClass(ptr, root){}

      _RevTreeIterator& operator=(const BaseClass& pIter)
      {
         return BaseClass(pIter);
      }

      ~_RevTreeIterator(){ BaseClass::_ptr = 0; BaseClass::_root = 0;}

      _Node& operator*() const{ return *BaseClass::_ptr; }

      _Node* operator->() const{return BaseClass::_ptr; }

      bool operator==(const _RevTreeIterator& pIter) const{ return BaseClass::_ptr == pIter._ptr; }

      bool operator!=(const _RevTreeIterator& pIter) const{ return !(BaseClass::_ptr == pIter._ptr); }

      _RevTreeIterator& operator--()
      {
         BaseClass::_ptr = BaseClass::_ptr->next();
         return *this;
      }

      _RevTreeIterator& operator++()
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
   class _FWDTreeChildIterator: public _TreeIteratorChildBase<_Node>
   {
   public:
      typedef _TreeIteratorChildBase<_Node> BaseClass;
      typedef _FWDTreeChildIterator<_Node> MyType;
      typedef _Node value_type;

      _FWDTreeChildIterator(){}

      _FWDTreeChildIterator(const BaseClass& pIter): BaseClass(pIter){}

      _FWDTreeChildIterator(value_type* ptr, value_type* root): BaseClass(ptr, root) {}

      _FWDTreeChildIterator& operator=(const BaseClass& pIter)
      {
         return BaseClass(pIter);
      }

      _Node& operator*() const{ return *BaseClass::_ptr; }

      _Node* operator->() const{return BaseClass::_ptr; }

      bool operator==(const _FWDTreeChildIterator& pIter) const{ return BaseClass::_ptr == pIter._ptr; }

      bool operator!=(const _FWDTreeChildIterator& pIter) const{ return !(BaseClass::_ptr == pIter._ptr); }

      _FWDTreeChildIterator& operator++()
      {
         BaseClass::_ptr = BaseClass::_ptr->next_sibling();
         return *this;
      }

      _FWDTreeChildIterator& operator--()
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
   class _RevTreeChildIterator: public _TreeIteratorChildBase<_Node>
   {
   public:
      typedef _TreeIteratorChildBase<_Node> BaseClass;
      typedef _RevTreeChildIterator<_Node> MyType;
      typedef _Node value_type;

      _RevTreeChildIterator(){}

      _RevTreeChildIterator(const BaseClass& pIter): BaseClass(pIter){}

      _RevTreeChildIterator(value_type* ptr, value_type* root): BaseClass(ptr, root){}

      _RevTreeChildIterator& operator=(const BaseClass& pIter)
      {
         return BaseClass(pIter);
      }

      _Node& operator*() const{ return *BaseClass::_ptr; }

      _Node* operator->() const{return BaseClass::_ptr; }

      bool operator==(const _RevTreeChildIterator& pIter) const{ return BaseClass::_ptr == pIter._ptr; }

      bool operator!=(const _RevTreeChildIterator& pIter) const{ return !(BaseClass::_ptr == pIter._ptr); }

      _RevTreeChildIterator& operator--()
      {
         BaseClass::_ptr = BaseClass::_ptr->next_sibling();
         return *this;
      }

      _RevTreeChildIterator& operator++()
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
   class _FWDTreeIterator: public _TreeIteratorBase<_Node>
   {
   public:
      typedef _TreeIteratorBase<_Node> BaseClass;
      typedef _FWDTreeIterator<_Node> MyType;
      typedef _Node value_type;

      _FWDTreeIterator(){}

      _FWDTreeIterator(const BaseClass& pIter): BaseClass(pIter){}

      _FWDTreeIterator(value_type* ptr, value_type* root): BaseClass(ptr, root){}

      _FWDTreeIterator& operator=(const BaseClass& pIter)
      {
         return BaseClass(pIter);
      }

      _Node& operator*() const{ return *BaseClass::_ptr; }

      _Node* operator->() const{return BaseClass::_ptr; }

      bool operator==(const _FWDTreeIterator& pIter) const{ return BaseClass::_ptr == pIter._ptr; }

      bool operator!=(const _FWDTreeIterator& pIter) const{ return !(BaseClass::_ptr == pIter._ptr); }

      _FWDTreeIterator& operator++()
      {
         BaseClass::_ptr = BaseClass::_ptr->next();
         return *this;
      }

      _FWDTreeIterator& operator--()
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
   class _FWDTreeIterator_Const: public _TreeIteratorBase_Const<_Node>
   {
   public:
      typedef _TreeIteratorBase_Const<_Node> BaseClass;
      typedef _FWDTreeIterator_Const<_Node> MyType;
      typedef _Node value_type;
      typedef const _Node* const_pointer;

      _FWDTreeIterator_Const(): BaseClass(){}

      _FWDTreeIterator_Const(const BaseClass& pIter): BaseClass(pIter){}
      _FWDTreeIterator_Const(const _TreeIteratorBase<_Node>& pIter): BaseClass(pIter){}

      _FWDTreeIterator_Const(const_pointer ptr, const_pointer root): BaseClass(ptr, root){}

      _FWDTreeIterator_Const& operator=(const _FWDTreeIterator_Const& pIter)
      {
         return BaseClass::operator=(pIter);
      }

      _FWDTreeIterator_Const& operator=(const _FWDTreeIterator<_Node>& pIter)
      {
         return BaseClass::operator=(pIter);
      }

      const _Node& operator*() const{ return *BaseClass::_ptr; }
      const _Node* operator->() const{return BaseClass::_ptr;}

      bool operator==(const _FWDTreeIterator_Const& pIter) const{ return BaseClass::_ptr == pIter._ptr; }

      bool operator!=(const _FWDTreeIterator_Const& pIter) const{ return !(BaseClass::_ptr == pIter._ptr); }

      _FWDTreeIterator_Const& operator++()
      {
         BaseClass::_ptr = BaseClass::_ptr->next();
         return *this;
      }

      _FWDTreeIterator_Const& operator--()
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
   class _RevTreeIterator_Const: public _TreeIteratorBase_Const<_Node>
   {
   public:
      typedef _TreeIteratorBase_Const<_Node> BaseClass;
      typedef _RevTreeIterator_Const<_Node> MyType;
      typedef _Node value_type;
      typedef const _Node* const_pointer;

      _RevTreeIterator_Const(): BaseClass(){}

      _RevTreeIterator_Const(const BaseClass& pIter): BaseClass(pIter){}
      _RevTreeIterator_Const(_TreeIteratorBase<_Node>& pIter): BaseClass(pIter){}

      _RevTreeIterator_Const(const_pointer ptr, const_pointer root): BaseClass(ptr, root){}

      _RevTreeIterator_Const& operator=(const _RevTreeIterator_Const& pIter)
      {
         return BaseClass::operator=(pIter);
      }

      _RevTreeIterator_Const& operator=(const _FWDTreeIterator<_Node>& pIter)
      {
         return BaseClass::operator=(pIter);
      }

      _RevTreeIterator_Const& operator=(const _RevTreeIterator<_Node>& pIter)
      {
         BaseClass::_ptr= pIter._ptr;
         BaseClass::_root = pIter._root;
         return *this;
      }

      const _Node& operator*() const{ return *BaseClass::_ptr; }
      const _Node* operator->() const{return BaseClass::_ptr;}

      bool operator==(const _RevTreeIterator_Const& pIter) const{ return BaseClass::_ptr == pIter._ptr; }

      bool operator!=(const _RevTreeIterator_Const& pIter) const{ return !(BaseClass::_ptr == pIter._ptr); }

      _RevTreeIterator_Const& operator--()
      {
         BaseClass::_ptr = BaseClass::_ptr->next();
         return *this;
      }

      _RevTreeIterator_Const& operator++()
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
   class _FWDTreeChildIterator_Const: public _TreeIteratorChildBase_Const<_Node>
   {
   public:
      typedef _FWDTreeChildIterator_Const<_Node> MyType;
      typedef _TreeIteratorChildBase_Const<_Node> BaseClass;
      typedef _Node value_type;
      typedef const _Node* const_pointer;

      _FWDTreeChildIterator_Const(): BaseClass(){}

      _FWDTreeChildIterator_Const(const BaseClass& pIter): BaseClass(pIter){}
      _FWDTreeChildIterator_Const(const _TreeIteratorChildBase<_Node>& pIter): BaseClass(pIter){}

      _FWDTreeChildIterator_Const(const_pointer ptr, const_pointer root): BaseClass(ptr, root){}

      _FWDTreeChildIterator_Const& operator=(const _FWDTreeChildIterator_Const& pIter)
      {
         return BaseClass::operator=(pIter);
      }

      _FWDTreeChildIterator_Const& operator=(const _FWDTreeIterator<_Node>& pIter)
      {
         return BaseClass::operator=(pIter);
      }

      _FWDTreeChildIterator_Const& operator=(const _RevTreeIterator<_Node>& pIter)
      {
         BaseClass::_ptr= pIter._ptr;
         BaseClass::_root = pIter._root;
         return *this;
      }

      const _Node& operator*() const{ return *BaseClass::_ptr; }

      const _Node* operator->() const{return BaseClass::_ptr;}

      bool operator==(const _FWDTreeChildIterator_Const& pIter) const{ return BaseClass::_ptr == pIter._ptr; }

      bool operator!=(const _FWDTreeChildIterator_Const& pIter) const{ return !(BaseClass::_ptr == pIter._ptr); }

      _FWDTreeChildIterator_Const& operator++()
      {
         BaseClass::_ptr = BaseClass::_ptr->next_sibling();
         return *this;
      }

      _FWDTreeChildIterator_Const& operator--()
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
   class _RevTreeChildIterator_Const: public _TreeIteratorChildBase_Const<_Node>
   {
   public:
      typedef _RevTreeChildIterator_Const<_Node> MyType;
      typedef _TreeIteratorChildBase_Const<_Node> BaseClass;
      typedef const _Node* const_pointer;

      _RevTreeChildIterator_Const(): BaseClass(){}

      _RevTreeChildIterator_Const(const BaseClass& pIter): BaseClass(pIter){}
      _RevTreeChildIterator_Const(const _TreeIteratorChildBase<_Node>& pIter): BaseClass(pIter){}

      _RevTreeChildIterator_Const(const_pointer ptr, const_pointer root): BaseClass(ptr, root){}

      _RevTreeChildIterator_Const& operator=(const _RevTreeChildIterator_Const& pIter)
      {
         return BaseClass::operator=(pIter);
      }

      _RevTreeChildIterator_Const& operator=(const _FWDTreeIterator<_Node>& pIter)
      {
         return BaseClass::operator=(pIter);
      }

      _RevTreeChildIterator_Const& operator=(const _RevTreeIterator<_Node>& pIter)
      {
         BaseClass::_ptr= pIter._ptr;
         BaseClass::_root = pIter._root;
         return *this;
      }

      _Node& operator*() const{ return *BaseClass::_ptr; }
      const _Node* operator->() const{return BaseClass::_ptr;}

      bool operator==(const _RevTreeChildIterator_Const& pIter) const{ return BaseClass::_ptr == pIter._ptr; }

      bool operator!=(const _RevTreeChildIterator_Const& pIter) const{ return !(BaseClass::_ptr == pIter._ptr); }

      _RevTreeChildIterator_Const& operator--()
      {
         BaseClass::_ptr = BaseClass::_ptr->next_sibling();
         return *this;
      }

      _RevTreeChildIterator_Const& operator++()
      {
         if(BaseClass::_ptr) BaseClass::_ptr = BaseClass::_ptr->prev_sibling();
         else BaseClass::_ptr = BaseClass::_root->last_child();
         return *this;
      }
   };

   template <class T>
   class Tree : public osg::Referenced
   {
      ////////////////////////////////////////////////////////////////////////////////////////////
      //Data Structures
      ////////////////////////////////////////////////////////////////////////////////////////////
   public:
      typedef Tree<T> _MyType;
      typedef _MyType TreeNode;

      typedef _FWDTreeIterator<TreeNode> iterator;
      typedef _FWDTreeIterator_Const<TreeNode> const_iterator;
      typedef _RevTreeIterator<TreeNode> reverse_iterator;
      typedef _RevTreeIterator_Const<TreeNode> const_reverse_iterator;

      typedef _FWDTreeChildIterator<TreeNode> child_iterator;
      typedef _FWDTreeChildIterator_Const<TreeNode> const_child_iterator;
      typedef _RevTreeChildIterator<TreeNode> reverse_child_iterator;
      typedef _RevTreeChildIterator_Const<TreeNode> const_reverse_child_iterator;

      typedef typename dtUtil::TypeTraits<T>::value_type value_type;
      typedef typename dtUtil::TypeTraits<T>::const_reference vt_const_ref;
      typedef typename dtUtil::TypeTraits<T>::pointer_type vt_ptr;

      typedef Tree<T>& reference;
      typedef const Tree<T>& const_reference;
      typedef Tree<T>* pointer;
      typedef const Tree<T>* const_pointer;
      typedef dtCore::RefPtr<Tree<T> > ref_pointer;

      /////////////////////////////////////////////////////////////////////////////////////////////
      //Functions
      /////////////////////////////////////////////////////////////////////////////////////////////
   public:
      Tree();
      Tree(T pData);
      Tree(const Tree&);
      Tree& operator=(const Tree&);

      //if this tree is derived overload this function and return an instance of your derived class
      virtual ref_pointer clone() const
      {
         return new Tree<T>();
      }

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

      ref_pointer mParent;
      ref_pointer mPrevSibling;
      ref_pointer mNext;
      ref_pointer mLastDecendant;
   };

   template <class T>
   Tree<T>::Tree()
      : value()
   {
      init();
   }

   template <class T>
   Tree<T>::Tree(T pData)
      : value(pData)
   {
      init();
   }

   template <class T>
   Tree<T>::Tree(const Tree<T>& pTree)
   {
      init();
      copy_children(&pTree);
   }

   template <class T>
   Tree<T>& Tree<T>::operator=(const Tree<T>& pTree)
   {
      clear();
      init();
      value = pTree.value;
      copy_children(&pTree);
      return *this;
   }

   template <class T>
   Tree<T>::~Tree()
   {
      mParent = NULL;
      mPrevSibling = NULL;
      mNext = NULL;
      mLastDecendant = NULL;
   }

   template <class T>
   void Tree<T>::init()
   {
      mParent = mNext = 0;
      mPrevSibling = mLastDecendant = this;
   }

   template <class T>
   void Tree<T>::clear()
   {
      destroy_decendants();

      mParent = NULL;
      mPrevSibling = NULL;
      mNext = NULL;
      mLastDecendant = NULL;
   }

   template <class T>
   bool Tree<T>::empty() const
   {
      return degree() == 0;
   }


   template <class T>
   unsigned Tree<T>::size() const
   {
      unsigned n = 1;
      for (const TreeNode* t = this; t != mLastDecendant.get(); t = t->next())
      {
         ++n;
      }

      return n;
   }

   template <class T>
   unsigned Tree<T>::level() const
   {
      unsigned l = 0;
      for (const TreeNode* t = mParent.get(); t != NULL; t = t->parent())
      {
         ++l;
      }

      return l;
   }

   template <class T>
   unsigned Tree<T>::degree() const
   {
      unsigned n = 0;
      for (const TreeNode* t = first_child(); t; t = t->next_sibling())
      {
         ++n;
      }
      return n;
   }

   template <class T>
   bool Tree<T>::is_leaf() const
   {
      return mLastDecendant.get() == this;
   }

   template <class T>
   bool Tree<T>::is_root() const
   {
      return mParent.valid();
   }

   template <class T>
   bool Tree<T>::is_descendant_of(const_reference ancestor) const
   {
      const TreeNode* t = this;
      for (; t; t = t->parent())
      {
         if (t == &ancestor)
         {
            break;
         }
      }
      return (t != 0);
   }

   template <class T>
   typename Tree<T>::pointer Tree<T>::prev() const
   {
      TreeNode* prev = 0;
      if(mParent.valid())
      {
         if(mParent->next() == this)
         {
            prev = mParent.get();
         }
         else
         {
            prev = mPrevSibling->mLastDecendant.get();
         }
      }

      return prev;
   }

   template <class T>
   typename Tree<T>::pointer Tree<T>::next() const
   {
      return mNext.get();
   }

   template <class T>
   typename Tree<T>::pointer Tree<T>::parent() const
   {
      return mParent.get();
   }

   template <class T>
   typename Tree<T>::pointer Tree<T>::first_child() const
   {
      TreeNode* child = 0;
      if(mNext.valid() && (mNext->mParent.get() == this))
      {
         child = next();
      }
      return child;
   }

   template <class T>
   typename Tree<T>::pointer Tree<T>::last_child() const
   {
      TreeNode* lastChild = first_child();
      if(lastChild)
      {
         lastChild = lastChild->mPrevSibling.get();
      }
      return lastChild;
   }

   template <class T>
   typename Tree<T>::pointer Tree<T>::next_sibling() const
   {
      TreeNode* nextSibling = mLastDecendant->next();
      if(nextSibling && nextSibling->mParent != mParent)
      {
         nextSibling = 0;
      }
      return nextSibling;
   }

   template <class T>
   typename Tree<T>::pointer Tree<T>::prev_sibling() const
   {
      TreeNode* prevSibling = 0;
      if(mParent.valid() && (mParent->next() != this))
      {
         prevSibling = mPrevSibling;
      }
      return prevSibling;
   }

   template <class T>
   typename Tree<T>::pointer Tree<T>::last_descendant() const
   {
      return mLastDecendant.get();
   }

   template <class T>
   typename Tree<T>::reference Tree<T>::front()
   {
      return *first_child();
   }

   template <class T>
   typename Tree<T>::reference Tree<T>::back()
   {
      return *last_child();
   }

   template <class T>
   typename Tree<T>::const_reference Tree<T>::front() const
   {
      return *first_child();
   }

   template <class T>
   typename Tree<T>::const_reference Tree<T>::back() const
   {
      return *last_child();
   }

   template <class T>
   void Tree<T>::pop_front()
   {
      ref_pointer first = first_child();
      erase(iterator(first.get(), this));
   }

   template <class T>
   void Tree<T>::pop_back()
   {
      ref_pointer last = last_child();
      erase(iterator(last.get(), this));
   }

   template <class T>
   void Tree<T>::push_front(ref_pointer subtree)
   {
      ref_pointer next = first_child();
      insert_subtree(subtree.get(), next);
   }

   template <class T>
   void Tree<T>::push_back(ref_pointer subtree)
   {
      insert_subtree(subtree.get(), 0);
   }

   template <class T>
   typename Tree<T>::iterator Tree<T>::erase(iterator pWhere)
   {
      ref_pointer child = &*pWhere;
      ref_pointer parent = child->mParent;
      ref_pointer next = child->next_sibling();
      parent->remove_subtree(child.get());
      child->clear();

      return iterator(next.get(), this);
   }

   template <class T>
   typename Tree<T>::reverse_iterator Tree<T>::erase(reverse_iterator pWhere)
   {
      ref_pointer child = &*pWhere;
      ref_pointer parent = child->mParent;
      ref_pointer rev_next = child->prev_sibling();
      parent->remove_subtree(child.get());
      child->clear();

      return reverse_iterator(rev_next.get(), this);
   }


   template <class T>
   typename Tree<T>::child_iterator Tree<T>::erase(child_iterator pWhere)
   {
      ref_pointer child = &*pWhere;
      ref_pointer parent = child->mParent;
      ref_pointer next = child->next_sibling();
      parent->remove_subtree(child.get());
      child->clear();

      return child_iterator(next.get(), this);
   }

   template <class T>
   typename Tree<T>::reverse_child_iterator Tree<T>::erase(reverse_child_iterator pWhere)
   {
      ref_pointer child = &*pWhere;
      ref_pointer parent = child->mParent;
      ref_pointer rev_next = child->prev_sibling();
      parent->remove_subtree(child.get());
      child->clear();

      return reverse_child_iterator(rev_next.get(), this);
   }

   template <class T>
   typename Tree<T>::iterator Tree<T>::insert(ref_pointer subtree, iterator pIter)
   {
      insert_subtree(subtree.get(), &*pIter);   // if end(), append to this
      return iterator(subtree.get(), this);
   }

   template <class T>
   typename Tree<T>::reverse_iterator Tree<T>::insert(ref_pointer subtree, reverse_iterator pIter)
   {
      insert_subtree(subtree.get(), &*pIter);   // if end(), append to this
      return reverse_iterator(subtree.get(), this);
   }

   template <class T>
   typename Tree<T>::child_iterator Tree<T>::insert(ref_pointer subtree, child_iterator pIter)
   {
      insert_subtree(subtree.get(), &*pIter);   // if end(), append to this
      return child_iterator(subtree.get(), this);
   }

   template <class T>
   typename Tree<T>::reverse_child_iterator Tree<T>::insert(ref_pointer subtree, reverse_child_iterator pIter)
   {
      insert_subtree(subtree.get(), &*pIter);   // if end(), append to this
      return reverse_child_iterator(subtree.get(), this);
   }

   template <class T>
   typename Tree<T>::iterator Tree<T>::begin()
   {
      return iterator(this, this);
   }

   template <class T>
   typename Tree<T>::const_iterator Tree<T>::begin() const
   {
      return const_iterator(this, this);
   }

   template <class T>
   typename Tree<T>::child_iterator Tree<T>::begin_child()
   {
      return typename Tree<T>::child_iterator(first_child(), this);
   }

   template <class T>
   typename Tree<T>::const_child_iterator Tree<T>::begin_child() const
   {
      return typename Tree<T>::const_child_iterator(first_child(), this);
   }

   template <class T>
   typename Tree<T>::reverse_iterator Tree<T>::rbegin()
   {
      return reverse_iterator(last_descendant(), this);
   }

   template <class T>
   typename Tree<T>::const_reverse_iterator Tree<T>::rbegin() const
   {
      return const_reverse_iterator(last_descendant(), this);
   }

   template <class T>
   typename Tree<T>::reverse_child_iterator Tree<T>::rbegin_child()
   {
      return reverse_child_iterator(last_child(), this);
   }

   template <class T>
   typename Tree<T>::const_reverse_child_iterator Tree<T>::rbegin_child() const
   {
      return const_reverse_child_iterator(last_child(), this);
   }

   template <class T>
   typename Tree<T>::iterator Tree<T>::end()
   {
      return iterator(0, this);
   }

   template <class T>
   typename Tree<T>::const_iterator Tree<T>::end() const
   {
      return const_iterator(0, this);
   }

   template <class T>
   typename Tree<T>::child_iterator Tree<T>::end_child()
   {
      return typename Tree<T>::child_iterator(0, this);
   }

   template <class T>
   typename Tree<T>::const_child_iterator Tree<T>::end_child() const
   {
      return typename Tree<T>::const_child_iterator(0, this);
   }

   template <class T>
   typename Tree<T>::reverse_iterator Tree<T>::rend()
   {
      return reverse_iterator(0, this);
   }

   template <class T>
   typename Tree<T>::const_reverse_iterator Tree<T>::rend() const
   {
      return const_reverse_iterator(0, this);
   }

   template <class T>
   typename Tree<T>::reverse_child_iterator Tree<T>::rend_child()
   {
      return reverse_child_iterator(0, this);
   }

   template <class T>
   typename Tree<T>::const_reverse_child_iterator Tree<T>::rend_child() const
   {
      return const_reverse_child_iterator(0, this);
   }

   template <class T>
   void Tree<T>::destroy_decendants()
   {
      mPrevSibling = mLastDecendant = this;
      if(mNext.valid() && mLastDecendant != this)
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
         mLastDecendant = this;
      }
   }

   template <class T>
   void Tree<T>::copy_children(const_pointer pTree)
   {
      for (const TreeNode* child = pTree->first_child(); child != NULL; child = child->next_sibling())
      {
         insert_subtree(child->clone(), 0);
      }
   }

   template <class T>
   void Tree<T>::change_last_descendant(pointer newLast)
   {
      ref_pointer oldLast = mLastDecendant;
      ref_pointer ancestor = this;
      do
      {
         ancestor->mLastDecendant = newLast;
         ancestor = ancestor->mParent;
      }
      while (ancestor.valid() && (ancestor->mLastDecendant == oldLast));
   }

   template <class T>
   void Tree<T>::remove_subtree(pointer child)
   {
      ref_pointer sib = child->next_sibling();
      if (sib.valid())
      {
         sib->mPrevSibling = child->mPrevSibling;
      }
      else
      {
         first_child()->mPrevSibling = child->mPrevSibling;
      }

      if (mLastDecendant == child->mLastDecendant)
      {
         change_last_descendant(child->prev());
      }

      if (mNext == child)   // deleting first child?
      {
         mNext = child->mLastDecendant->mNext;
      }
      else
      {
         child->mPrevSibling->mLastDecendant->mNext = child->mLastDecendant->mNext;
      }
   }

   template <class T>
   void Tree<T>::insert_subtree(pointer pSubTree, pointer pNext)
   {
      if (pNext == NULL)
      {
         // append as last child
         pSubTree->mParent = this;
         pSubTree->mLastDecendant->mNext = mLastDecendant->mNext;
         mLastDecendant->mNext = pSubTree;

         pSubTree->mPrevSibling = last_child();
         if (is_leaf())
         {
            mNext = pSubTree;
         }

         first_child()->mPrevSibling = pSubTree;

         change_last_descendant(pSubTree->mLastDecendant);
      }
      else
      {
         ref_pointer parent = pNext->mParent;
         pSubTree->mParent = parent;
         pSubTree->mPrevSibling = pNext->mPrevSibling;

         pSubTree->mLastDecendant->mNext = pNext;
         if (parent->mNext == pNext)   // inserting before first subtree?
         {
            parent->mNext = pSubTree;
         }
         else
         {
            pNext->mPrevSibling->mLastDecendant->mNext = pSubTree;
         }

         pNext->mPrevSibling = pSubTree;
      }
   }

} // namespace dtAI

#endif //DTAI_DELTA_TREE_H
