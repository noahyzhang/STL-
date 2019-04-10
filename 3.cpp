#if 0
#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;

int main()
{	
	vector<long long int> vec;
	int num = 0;
	while (cin >> num)
		vec.push_back(num);
	int len = vec.size();
	sort(vec.begin(),vec.end());

	int mid = len / 2;
	long long int midNum = vec[mid];
	int count = 0;

	for (int i = 0; i < len; ++i)
	{
		if (vec[i] == midNum)
			continue;
		else if (vec[i] < midNum)
		{
			if (vec[i] == 1)
				count++;
			else
				count += (midNum - vec[i]);
		}
		else
		{
			count += (vec[i] - midNum);
		}
	}
	cout << count << endl;
	return 0;
}
#endif