#pragma once
#include"huffmanTree.h"
#define  _CRT_SECURE_NO_WARNINGS

typedef long long longType;
struct FileInfo
{
	unsigned char _ch;		// 字符
	longType _appearCount;	// 出现次数
	string _code;	// 字符对应的huffman编码

	FileInfo(int appearCount = 0)
		:_ch(0)
		, _appearCount(appearCount)
	{}
	bool operator < (const FileInfo& info) const
	{
		return _appearCount < info._appearCount;
	}

	bool operator != (const FileInfo& info) const
	{
		return _appearCount != info._appearCount;
	}

	FileInfo operator +(const FileInfo& info)
	{
		FileInfo tmp;
		tmp._appearCount = _appearCount + info._appearCount;
		return tmp;
	}

};
ostream& operator<<(ostream& os, const FileInfo& info)
{
	os << info._ch << ":" << info._appearCount;
	return os;
}
class FileCompress
{
public:
	FileCompress()
	{
		for (int i = 0; i < 256; ++i)
		{
			_fileInfos[i]._ch = i;
		}
	}
	bool ReadLine(FILE* fOut, string& line)
	{
		assert(fOut);
		char ch = fgetc(fOut);
		if (ch == EOF)
			return false;
		while (ch != '\0' || ch != EOF)
		{
			line += ch;
			ch = fgetc(fOut);
		}
		return true;
	}
	void Compress(const char* fileName)
	{
		assert(fileName);
		longType charCount = 0;
		FILE* fOut = fopen(fileName, "rb");
		char ch = fgetc(fOut);
		while (ch != EOF)
		{
			_fileInfos[(unsigned char)ch]._appearCount++;
			ch = fgetc(fOut);
			++charCount;
		}
		HuffmanTree<FileInfo> tree;
		FileInfo invalid(0);
		tree.CreateHuffmanTree(_fileInfos, 256, 0);
		_GenerateHuffmanCode(tree.GetRootNode());
		string compressFileName = fileName;
		compressFileName += ".Compress";
		FILE* fIn = fopen(compressFileName.c_str(), "wb");
		fseek(fOut, 0, SEEK_SET);//从头开始读取
		ch = fgetc(fOut);
		int pos = 0;
		char value = 0;
		while (ch != EOF)
		{
			string& code = _fileInfos[(unsigned char)ch]._code;
			for (size_t i = 0; i < code.size(); ++i)
			{
				value <<= 1;

				if (code[i] == '1')
				{
					value |= 1;
				}

				if (++pos == 8)
				{
					fputc(value, fIn);
					pos = 0;
					value = 0;
				}
			}

			ch = fgetc(fOut);
		}

		if (pos)
		{
			value <<= (8 - pos);
			fputc(value, fIn);
		}

		// 4.将Huffman树的信息写入配置文件。
		string configFileName = fileName;
		configFileName += ".config";
		FILE* fInConfig = fopen(configFileName.c_str(), "wb");
		assert(fInConfig);
		string infoStr;
		char countStr[20];

		// 先写入字符个数
		_itoa(charCount >> 32, countStr, 10);//charCount  文件总字符次数
		fputs(countStr, fInConfig);
		fputc('\n', fInConfig);

		_itoa(charCount & 0xFFFFFFFF, countStr, 10);
		fputs(countStr, fInConfig);
		fputc('\n', fInConfig);

		for (int i = 0; i < 256; ++i)
		{
			if (_fileInfos[i] != invalid)
			{
				infoStr = _fileInfos[i]._ch;
				infoStr += ',';
				_itoa(_fileInfos[i]._appearCount, countStr, 10);
				infoStr += countStr;
				infoStr += '\n';

				fputs(infoStr.c_str(), fInConfig);
			}
		}

		fclose(fIn);
		fclose(fOut);
		fclose(fInConfig);
	}
	void Uncompress(const char* fileName)
	{
		// 1.读取配置文件中Huffman树的信息
		string configFileName = fileName;
		configFileName += ".config";
		FILE* fOutConfig = fopen(configFileName.c_str(), "rb");
		string line;
		char ch = 0;
		longType appearCount = 0;

		// 读取文件的字符个数
		longType charCount = 0;
		ReadLine(fOutConfig, line);
		charCount = atoi(line.c_str());
		charCount <<= 32;
		ReadLine(fOutConfig, line);
		charCount += atoi(line.c_str());
		line.clear();

		while (ReadLine(fOutConfig, line))
		{
			// 若读到一个空行，则对应字符为换行符
			if (!line.empty())
			{
				//sscanf(line.c_str(), "%s,%d", ch, appearCount);
				ch = line[0];
				_fileInfos[(unsigned char)ch]._appearCount = atoi(line.substr(2).c_str());
				line.clear();
			}
			else
			{
				line = '\n';
			}
		}

		// 2.根据读出的配置信息，重建Huffman树
		HuffmanTree<FileInfo> tree;
		FileInfo invalid(0);
		tree.CreateHuffmanTree(_fileInfos, 256, invalid);
		HuffmanNode<FileInfo>* root = tree.GetRootNode();

		// 3. 读取压缩信息，根据重建的Huffman树解压缩
		string uncompressFileName = fileName;
		uncompressFileName += ".Uncompress";
		FILE* fIn = fopen(uncompressFileName.c_str(), "wb");

		string compressFileName = fileName;
		compressFileName += ".Compress";
		FILE* fOut = fopen(compressFileName.c_str(), "rb");

		// 3.1根据压缩文件字符编码再Huffman树中寻找对应的字符
		int pos = 8;
		HuffmanNode<FileInfo>* cur = root;
		ch = fgetc(fOut);
		while (charCount > 0)
		{
			//找到叶子节点
			while (cur && cur->_left && cur->_right)
			{
				if (pos == 0)
				{
					pos = 8;
					ch = fgetc(fOut);
				}

				--pos;

				if (ch & 1 << pos)//得到ch中第pos位
				{
					//
					cur = cur->_right;
				}
				else
				{
					cur = cur->_left;
				}
			}
			//cur是叶子节点，
			if (cur && charCount--)
			{
				fputc(cur->_weight._ch, fIn);
				cur = root;
			}
		}

		cout << endl;

		fclose(fIn);
		fclose(fOut);
		fclose(fOutConfig);
	}
protected:
	void _GenerateHuffmanCode(HuffmanNode<FileInfo>* root)
	{
		if (root)
		{
			_GenerateHuffmanCode(root->_left);
			_GenerateHuffmanCode(root->_right);

			// 叶子节点则为编码节点，顺着叶子节点向上寻找Huffman编码
			if (root->_left == NULL && root->_right == NULL)
			{
				// code是_fileInfos数组中编码字符串对象的引用。
				string& code = _fileInfos[root->_weight._ch]._code;

				HuffmanNode<FileInfo>* cur = root;
				HuffmanNode<FileInfo>* parent = root->_parent;
				while (parent)
				{
					if (parent->_left == cur)
					{
						code += '0';
					}
					else
					{
						code += '1';
					}

					cur = parent;
					parent = cur->_parent;
				}

				// 寻找编码是从叶子节点到根节点，所以要对编码进行逆置
				reverse(code.begin(), code.end());

			}
		}
	}

protected:
	FileInfo _fileInfos[256];
};
void TestCompress()
{
	FileCompress fc;
	//int begin = GetTickCount();

	// 压缩
	fc.Compress("in.txt");


	//int end = GetTickCount();
	//cout << "Compress:" << end - begin << endl;


}
void TestUncompress()
{
	FileCompress fc;
	//int	begin = GetTickCount();

	// 解压缩

	fc.Uncompress("in.txt");


	//int end = GetTickCount();
	//cout << "Uncompress:" << end - begin << endl;
}