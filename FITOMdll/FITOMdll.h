// 以下の ifdef ブロックは DLL からのエクスポートを容易にするマクロを作成するための 
// 一般的な方法です。この DLL 内のすべてのファイルは、コマンド ラインで定義された FITOMDLL_EXPORTS
// シンボルを使用してコンパイルされます。このシンボルは、この DLL を使用するプロジェクトでは定義できません。
// ソースファイルがこのファイルを含んでいる他のプロジェクトは、 
// FITOMDLL_API 関数を DLL からインポートされたと見なすのに対し、この DLL は、このマクロで定義された
// シンボルをエクスポートされたと見なします。
#ifdef FITOMDLL_EXPORTS
#define FITOMDLL_API __declspec(dllexport)
#else
#define FITOMDLL_API __declspec(dllimport)
#endif

// このクラスは FITOMdll.dll からエクスポートされました。
class FITOMDLL_API CFITOMdll {
public:
	CFITOMdll(void);
	// TODO: メソッドをここに追加してください。
};

extern FITOMDLL_API int nFITOMdll;

FITOMDLL_API int fnFITOMdll(void);
