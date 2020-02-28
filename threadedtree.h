#include <iostream>
using namespace std;

template <class T>
class ThreadedTree{
	struct Node{
		T data_;
		Node* left_;
		Node* right_;
		bool leftIsThread_;
		bool rightIsThread_;
		Node(const T& data=T{},Node* left=nullptr, Node* right=nullptr){
			data_=data;
			left_=left;
			right_=right;
			leftIsThread_=true;
			rightIsThread_=true;
		}
	};
	Node* root_;	
	void print(const Node* rt, int lvl)const{
		if(rt){
			Node* left = rt->leftIsThread_? nullptr : rt->left_;
			print(left, lvl+1);
			for(int i =0; i<lvl; i++){
				std::cout << "---";
			}
			Node* right = rt->rightIsThread_? nullptr : rt->right_;
				std::cout << rt->data_ << endl;
			print(right, lvl+1);	
		}
	}

public:
	class const_iterator{
	protected:
		Node* curr_;
		const_iterator(Node* c){curr_=c;}
	public:
		const_iterator(){
			curr_=nullptr;
		}
		const_iterator operator++(int){
			Node* prev = this->curr_;
			if(curr_ != nullptr){
			curr_ = curr_->right_;
				if(prev->rightIsThread_ == false){
					while(curr_->leftIsThread_ == false){
						curr_ = curr_->left_;
					}
				}
			return const_iterator(prev);
			}
		}
		const_iterator operator--(int){
			Node* prev = this->curr_;
			if(prev){
				if(curr_->left_ != nullptr){
				curr_ = curr_->left_;
					if(prev->leftIsThread_ == false){
						while(curr_->rightIsThread_ == false){
							curr_ = curr_->right_;
						}
					}
				}
				return const_iterator(prev);
			}
		}
		const T& operator*(){
			return curr_->data_;
		}
		bool operator==(const const_iterator& rhs) const{
			return curr_ == rhs.curr_;
		}
		bool operator!=(const const_iterator& rhs) const{
			return curr_ != rhs.curr_;
		}
		friend class ThreadedTree;
	};
	class iterator:public const_iterator{
		iterator(Node* c):const_iterator(c){}
	public:
		iterator():const_iterator(){}
		const T& operator*(){
			return this->curr_->data_;
		}
		iterator operator++(int){
			Node* prev = this->curr_;
			if(this->curr_!=nullptr){
			this->curr_ = this->curr_->right_;
				if(prev->rightIsThread_ == false){
					while(this->curr_->leftIsThread_ == false){
						this->curr_ = this->curr_->left_;
					}
				}
			return iterator(prev);
			}
		}
		iterator operator--(int){
			//std::cout << "-- start" << std::endl;
//			std::cout << "not const" << std::endl;
		Node* prev = this->curr_;
		if(prev){
			if(this->curr_->left_ != nullptr){
				//std::cout << "I'm in" << std::endl;
				this->curr_ = this->curr_->left_;
					if(prev->leftIsThread_ == false){
					//	std::cout << "going right" << std::endl;
						while(this->curr_->rightIsThread_ == false){
					//		std::cout << "+1 right" << std::endl;
							this->curr_ = this->curr_->right_;
						}
					}
					//std::cout << "Let's get out of here" << std::endl;
				}
				return iterator(prev);
			}
		}
		friend class ThreadedTree;
	};

	ThreadedTree(){
		root_=nullptr;
	}

	iterator insert(const T& data){
		if(root_ == nullptr){
			root_ = new Node(data);
			return iterator(root_);
		}
		Node* tn = root_;
		bool esc = false;

		while(!esc){

			if(data > tn->data_){
				if(tn->rightIsThread_){
					//create the new greater than node
					Node* nn = new Node(data);
					//transfer the right thread to the greater value node
					nn->right_ = tn->right_;
					//point previous node to the new greater node instead of thread
					tn->right_ = nn;
					tn->rightIsThread_ = false;
					//make new node point to less than node for left thread
					nn->left_ = tn;
					return iterator(nn);
					esc=true;				}
				else{
					//if the next node doesn't exist, make the new node there
					tn = tn->right_;
				}
			}
			else if(data < tn->data_){
				if(tn->leftIsThread_){
					//create the new less than node
					Node* nn = new Node(data);
					//transfer the left thread to the lower value node
					nn->left_ = tn->left_;
					//point previous node to the new lower node instead of thread
					tn->left_ = nn;
					tn->leftIsThread_ = false;
					//make new node point to greater than node for right thread
					nn->right_ = tn;

					return iterator(nn);
					esc = true;
				}
				else{
					//if the next node doesn't exist, make the new node there
					tn = tn->left_;
				}
			}
		}
	} 

	iterator find(const T& data){
		Node* n = root_;
		while(n->data_ != data){
			if(data < n->data_){
				n = n->left_;
			}
			else if(data > n->data_){
				n = n->right_;
			}
		}
	return iterator(n);
	}
	iterator begin(){
//		std::cout << "begin called" << std::endl;
		Node* n = nullptr;
			if(root_ != nullptr){
			n = root_;
			while(n->leftIsThread_ == false)
				n = n->left_;
			}
		return iterator(n);
	}
	iterator end(){
	return iterator(nullptr);
	}
	const_iterator begin()const{
//		std::cout << "begin called" << std::endl;
	Node* n = nullptr;
			if(root_ != nullptr){
			n = root_;
			while(n->leftIsThread_ == false)
				n = n->left_;
			}
		return const_iterator(n);
	}
	const_iterator end() const{
	return const_iterator(nullptr);
	}
	void print() const{
		print(root_,0);
	}

	void leftdel(Node* rt){
		if(rt->leftIsThread_ != false){
			Node* left = rt->left_;
			leftdel(left);
		}
		delete [] rt;
	}
	void rightdel(Node* rt){
		if(rt->right_ != nullptr){
			Node* right_ = rt->right_;
			leftdel(right_);
		}
		delete [] rt;
	}
	~ThreadedTree(){

		if(root_){
			Node* left;
			leftdel(root_->left_);
			rightdel(root_->right_);
			delete [] root_;
		}
	}
};
