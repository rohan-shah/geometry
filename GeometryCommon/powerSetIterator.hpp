#include <vector>
namespace geometry
{
	template<class T, class U> class zipIterator 
	{
	public:
		void operator=(zipIterator<T, U>&& other)
		{
			if(&input1 != &other.input1 || &input2 != &other.input2)
			{
				throw std::runtime_error("Attempt to change underlying vector in zipIterator");
			}
			indices.swap(other.indices);
			n = other.n;
			size = other.size;
			isDone = other.isDone;
		}
		zipIterator(zipIterator<T, U>&& other)
			:input1(other.input1), input2(other.input2), indices(other.indices), n(other.n), size(other.size), isDone(other.isDone)
		{};
		zipIterator(const std::vector<T>& input1, const std::vector<U>& input2, std::size_t n)
			:input1(input1), input2(input2), indices(n), n(n), size(input1.size() + input2.size()), isDone(false)
		{
			if(size == 0)  
			{
				isDone = true;
			}
			else
			{
				for(int i = 0; i < (int)n; i++) indices[i] = i;
			}
		};
		void extract(std::vector<T>& output1, std::vector<U>& output2) const
		{
#ifndef NDEBUG
			if(isDone) throw std::runtime_error("Iterating past end of powerSetIterator");
#endif
			output1.clear();
			output2.clear();
			for(int i = 0; i < (int)n; i++)
			{
				if(indices[i] >= input1.size())
				{
					output2.push_back(input2[indices[i] - input1.size()]);
				}
				else
				{
					output1.push_back(input1[indices[i]]);
				}
			}
		};
		bool done() const
		{
			return isDone;
		};
		void operator++()
		{
#ifndef NDEBUG
			if(isDone) throw std::runtime_error("Iterating past end of powerSetIterator");
#endif
			std::size_t expected = size - 1;
			for(std::size_t index = n; index>= 1; index--)
			{
				if(indices[index-1] != expected)
				{
					indices[index-1]++;
					for(std::size_t k = index; k < n; k++)
					{
						indices[k] = indices[k-1]+1;
					}
					return;
				}
				expected--;
			}
			isDone = true;
		}
		void operator++(int)
		{
			++(*this);
		}
	private:
		const std::vector<T>& input1;
		const std::vector<U>& input2;
		std::vector<std::size_t> indices;
		std::size_t n;
		std::size_t size;
		bool isDone;
		zipIterator(const zipIterator<T, U>& other)
		{};
	};
	template<class T, class U> class allSubsetsIterator
	{
	public:
		allSubsetsIterator(const std::vector<T>& input1, const std::vector<U>& input2)
			: isDone(false), iterator(input1, input2, 1), n(1), size(input1.size() + input2.size()), input1(input1), input2(input2)
		{
			if(size == 0) isDone = true;
		}
		bool done() const
		{
			return isDone;
		}
		void operator++()
		{
#ifndef NDEBUG
			if(isDone) throw std::runtime_error("Iterating past end of powerSetIterator");
#endif
			iterator++;
			if(iterator.done())
			{
				n++;
				if(n == size+1) isDone = true;
				else iterator = zipIterator<T, U>(input1, input2, n);
			}
		}
		void operator++(int)
		{
			++(*this);
		}
		void extract(std::vector<T>& output1, std::vector<U>& output2) const
		{
			iterator.extract(output1, output2);
		}
	private:
		bool isDone;
		zipIterator<T, U> iterator;
		std::size_t n;
		std::size_t size;
		const std::vector<T>& input1;
		const std::vector<U>& input2;
	};
	template<class T, class U> class smartSubsetsIterator
	{
	public:
		smartSubsetsIterator(const std::vector<T>& input1, const std::vector<U>& input2)
			:input1(input1), input2(input2), size(input1.size() + input2.size()), isDone(false)
		{
			if(size == 0) isDone = true;
			indices.push_back(0);
		}
		bool done() const
		{
			return isDone;
		}
		void operator++()
		{
#ifndef NDEBUG
			if(isDone) throw std::runtime_error("Iterating past end of smartSubsetsIterator");
#endif
			int removed = 0;
			while(indices.size() > 0 && *indices.rbegin() == size-removed-1)
			{
				indices.pop_back();
				removed++;
			}
			isDone = indices.size() == 0;
			if(!isDone)
			{
				indices.push_back(*indices.rbegin()+1);
			}
		}
		void skip()
		{
			int removed = 0;
			while(*indices.rbegin() == size-removed)
			{
				indices.pop_back();
				removed++;
			}
			isDone = indices.size() == 0;
			if(!isDone)
			{
				(*indices.rbegin())++;
			}
		}
		void operator++(int)
		{
			++(*this);
		}
		void extract(std::vector<T>& output1, std::vector<U>& output2) const
		{
			output1.clear();
			output2.clear();
			for(int i = 0; i < indices.size(); i++)
			{
				if(indices[i] >= input1.size())
				{
					output2.push_back(input2[indices[i] - input1.size()]);
				}
				else
				{
					output1.push_back(input1[indices[i]]);
				}
			}
		}
	private:
		bool isDone;
		std::size_t n;
		std::size_t size;
		const std::vector<T>& input1;
		const std::vector<U>& input2;
		std::vector<std::size_t> indices;
	};
}
