#pragma once

namespace SCCISharp {

	using namespace System;
	using namespace System::Collections::Generic;

	ref class InterfaceInfo;
	ref class SoundChipInfo;
	ref class SoundInterface;
	ref class SoundInterfaceManager;
	ref class SoundChip;

	/// <summary>
	/// チップの種類を表す列挙体です。
	/// </summary>
	public enum class ChipType
	{
		None = 0,
		YM2608,
		YM2151,
		YM2610,
		YM2203,
		YM2612,
		AY8910,
		SN76489,
		YM3812,
		YMF262,
		YM2413,
		YM3526,
		YMF288,
		SCC,
		SCCS,
		Y8950,
		YM2164,
		YM2414,
		AY8930,
		YM2149,
		YMZ294,
		SN76496,
		YM2420,
		YMF281,
		YMF276,
		YM2610B,
		YMF286,
		YM2602,
		UM3567,
		YMF274,
		YM3806,
		YM2163,
		YM7129,
		YMZ280,
		YMZ705,
		YMZ735,
		SPC700,
		MAX
	};

	/// <summary>
	/// チップのロケーションを区別するための列挙体です。
	/// </summary>
	public enum class SoundLocation
	{
		/// <summary>
		/// モノラル。
		/// </summary>
		Mono = 0,

		/// <summary>
		/// 左チャネル。
		/// </summary>
		Left = 1,

		/// <summary>
		/// 右チャネル。
		/// </summary>
		Right = 2,

		/// <summary>
		/// ステレオ。
		/// </summary>
		Stereo = 3,
	};

	/// <summary>
	/// インタフェースとの通信方式を表す列挙体です。
	/// </summary>
	public enum class CommunicationMode
	{
		/// <summary>
		/// 通信は非同期的に行われます。
		/// </summary>
		Async = 0x00000000,

		/// <summary>
		/// 通信は同期的に行われます。
		/// </summary>
		Sync = 0x00000001,
	};

	/// <summary>
	/// チップの取得方法を表す列挙体です。
	/// </summary>
	public enum class AcquisitionMode
	{
		/// <summary>
		/// チップは指定されたタイプとクロックに最も近いものが選択され、取得されます。
		/// </summary>
		Near = 0x00000000,

		/// <summary>
		/// チップは指定されたタイプとクロックに一致するものが選択され、取得されます。
		/// ただし、クロックは SetAcquisitionModeClockRange メソッドで指定された範囲の中から選択されます。
		/// </summary>
		Match = 0x00000001,
	};

	/// <summary>
	/// インタフェースの状態を格納したクラスです。
	/// </summary>
	public ref class InterfaceInfo
	{
	public:
		/// <summary>
		/// インタフェースの名前を表す文字列を取得します。
		/// </summary>
		property String^ Name { String^ get(); }

		/// <summary>
		/// インタフェースに接続されているチップの総数を取得します。
		/// </summary>
		property Int32 SoundChipCount { Int32 get(); }

	internal:
		InterfaceInfo(String^ name, Int32 chipCount);

	private:
		String^ name;
		Int32 chipCount;
	};

	/// <summary>
	/// チップの状態を格納したクラスです。
	/// </summary>
	public ref class SoundChipInfo
	{
	public:
		/// <summary>
		/// チップの名前を表す文字列を取得します。
		/// </summary>
		property String^ Name { String^ get(); }

		/// <summary>
		/// チップの種類を表す <see cref="SCCISharp::ChipType"/> 列挙体を取得します。
		/// </summary>
		property ChipType Type { ChipType get(); }

		/// <summary>
		/// このチップと互換性のあるチップの種類を表す <see cref="SCCISharp::ChipType"/> 列挙体のリストを取得します。
		/// </summary>
		property IReadOnlyList<ChipType>^ CompatibleChipType { IReadOnlyList<ChipType>^ get(); }

		/// <summary>
		/// チップのクロック周波数 (Hz) を取得します。
		/// </summary>
		property Int32 Clock { Int32 get(); }

		/// <summary>
		/// このチップと互換性のあるチップのクロック周波数 (Hz) のリストを取得します。
		/// </summary>
		property IReadOnlyList<Int32>^ CompatibleChipClock { IReadOnlyList<Int32>^ get(); }

		/// <summary>
		/// チップが使用中であるかを表す真偽値を取得します。
		/// </summary>
		property bool IsUsed { bool get(); }

		/// <summary>
		/// チップが接続されているスロットの ID を取得します。
		/// </summary>
		property Int32 Slot { Int32 get(); }

		/// <summary>
		/// チップのロケーションを表す <see cref="SCCISharp::SoundLocation"/> 列挙体を取得します。
		/// </summary>
		property SCCISharp::SoundLocation SoundLocation { SCCISharp::SoundLocation get(); }

	internal:
		SoundChipInfo(String^ name,
					  ChipType chipType,
					  IReadOnlyList<ChipType>^ compatibleChip,
					  Int32 clock,
					  IReadOnlyList<Int32>^ compatibleChipClock,
					  bool isUsed,
					  Int32 slot,
					  SCCISharp::SoundLocation soundLocation);

	private:
		String^ name;
		ChipType chipType;
		IReadOnlyList<ChipType>^ compatibleChip;
		Int32 clock;
		IReadOnlyList<Int32>^ compatibleChipClock;
		bool isUsed;
		Int32 slot;
		SCCISharp::SoundLocation soundLocation;
	};

	/// <summary>
	/// インタフェースを管理するためのマネージャクラスです。
	/// </summary>
	public ref class SoundInterfaceManager : IDisposable
	{
	public:
		/// <summary>
		/// マネージャのバージョンを表す数値を取得します。
		/// </summary>
		/// <value>バージョンを表す数値。</value>
		property Int32 Version { Int32 get(); }

		/// <summary>
		/// ディレイ時間 (ms) を取得または設定します。
		/// </summary>
		/// <value>ディレイ時間を表す 0 以上の数値。</value>
		property Int32 Delay { Int32 get(); void set(Int32 value); }

		/// <summary>
		/// レベルディスプレイが表示されているかを表す真偽値を取得します。
		/// </summary>
		/// <value>表示されているとき true、それ以外のとき false。</value>
		property bool ShownLevelDisplay { bool get(); }

		/// <summary>
		/// 接続されているインタフェースの総数を取得します。
		/// </summary>
		/// <value>インタフェースの総数を表す 0 以上の数値。</value>
		property Int32 InterfaceCount { Int32 get(); }

		/// <summary>
		/// オブジェクトが解放されたかを表す真偽値を取得します。
		/// </summary>
		/// <value>解放されているとき true、それ以外のとき false。</value>
		property bool IsDisposed { bool get(); }

		/// <summary>
		/// 指定されたインタフェースの状態を格納した <see cref="SCCISharp::InterfaceInfo"/> オブジェクトを取得します。
		/// </summary>
		/// <param name="interfaceNumber">インタフェースの番号。</param>
		/// <returns><see cref="SCCISharp::InterfaceInfo"/> オブジェクト。</returns>
		InterfaceInfo^ GetInterfaceInfo(Int32 interfaceNumber);

		/// <summary>
		/// 指定されたインタフェースを表す <see cref="SCCISharp::SoundInterface"/> オブジェクトを取得します。
		/// </summary>
		/// <param name="interfaceNumber">インタフェースの番号。</param>
		/// <returns><see cref="SCCISharp::SoundInterface"/> オブジェクト。</returns>
		SoundInterface^ GetInterface(Int32 interfaceNumber);

		/// <summary>
		/// 全てのインタフェースのリソースを解放します。
		/// </summary>
		/// <returns>解放に成功したとき true、それ以外のとき false。</returns>
		bool ReleaseAllInterfaces();

		/// <summary>
		/// 指定されたチップを表す <see cref="SCCISharp::SoundChip"/> オブジェクトを取得します。
		/// </summary>
		/// <param name="soundChipType">チップの種類を表す <see cref="SCCISharp::ChipType"/> の値。</param>
		/// <param name="clock">チップのクロック周波数 (Hz)。</param>
		/// <returns><see cref="SCCISharp::SoundChip"/> オブジェクト。</returns>
		SoundChip^ GetSoundChip(ChipType soundChipType, Int32 clock);

		/// <summary>
		/// 指定されたチップを表す <see cref="SCCISharp::SoundChip"/> オブジェクトを取得します。
		/// </summary>
		/// <param name="soundChipType">チップの種類を表す <see cref="SCCISharp::ChipType"/> の値。</param>
		/// <param name="clockMhz">チップのクロック周波数 (MHz)。</param>
		/// <returns><see cref="SCCISharp::SoundChip"/> オブジェクト。</returns>
		SoundChip^ GetSoundChip(ChipType soundChipType, double clockMhz);
		
		/// <summary>
		/// 指定されたチップを表す <see cref="SCCISharp::SoundChip"/> オブジェクトを取得し、
		/// 取得に成功したかの真偽値を返却します。
		/// </summary>
		/// <param name="soundChip">チップを表す <see cref="SCCISharp::SoundChip"/> オブジェクト。このパラメータは参照渡しされます。</param>
		/// <param name="soundChipType">チップの種類を表す <see cref="SCCISharp::ChipType"/> の値。</param>
		/// <param name="clock">チップのクロック周波数 (Hz)。</param>
		/// <returns>取得に成功したとき true、それ以外のとき false。</returns>
		bool TryGetSoundChip([System::Runtime::InteropServices::Out] SoundChip^% soundChip, ChipType soundChipType, Int32 clock);

		/// <summary>
		/// 指定されたチップを表す <see cref="SCCISharp::SoundChip"/> オブジェクトを取得し、
		/// 取得に成功したかの真偽値を返却します。
		/// </summary>
		/// <param name="soundChip">チップを表す <see cref="SCCISharp::SoundChip"/> オブジェクト。このパラメータは参照渡しされます。</param>
		/// <param name="soundChipType">チップの種類を表す <see cref="SCCISharp::ChipType"/> の値。</param>
		/// <param name="clockMhz">チップのクロック周波数 (MHz)。</param>
		/// <returns>取得に成功したとき true、それ以外のとき false。</returns>
		bool TryGetSoundChip([System::Runtime::InteropServices::Out] SoundChip^% soundChip, ChipType soundChipType, double clockMhz);

		/// <summary>
		/// 全てのチップのリソースを解放します。
		/// </summary>
		/// <returns>解放に成功したとき true、それ以外のとき false。</returns>
		bool ReleaseAllSoundChips();

		/// <summary>
		/// レベルディスプレイを表示します。
		/// </summary>
		void ShowLevelDisplay();

		/// <summary>
		/// レベルディスプレイを閉じます。
		/// </summary>
		void CloseLevelDisplay();

		/// <summary>
		/// インタフェースマネージャをリセットします。
		/// </summary>
		/// <returns>リセットに成功したとき true、それ以外のとき false。</returns>
		bool Reset();

		/// <summary>
		/// インタフェースマネージャを初期化します。
		/// </summary>
		/// <returns>初期化に成功したとき true、それ以外のとき false。</returns>
		bool Initialize();

		/// <summary>
		/// インタフェースとの通信モードを設定します。
		/// </summary>
		/// <param name="mode">通信モードを表す <see cref="SCCISharp::CommunicationMode"/> の値。</param>
		void SetMode(CommunicationMode mode);

		/// <summary>
		/// バッファ内のデータを即時に送信します。
		/// </summary>
		void FlushData();

		/// <summary>
		/// バッファをクリアします。
		/// </summary>
		void ClearBuffer();

		/// <summary>
		/// チップの取得モードを指定します。
		/// </summary>
		/// <param name="mode">取得モードを表す <see cref="SCCISharp::AcquisitionMode"/> の値。</param>
		void SetAcquisitionMode(AcquisitionMode mode);

		/// <summary>
		/// チップの取得モードが <see cref="SCCISharp::AcquisitionMode::Match"/> であったときに
		/// 許容されるクロック周波数 (Hz) の幅を指定します。
		/// </summary>
		/// <param name="clock">許容されるクロック周波数 (Hz) の幅。</param>
		void SetAcquisitionModeClockRange(Int32 clock);

		/// <summary>
		/// 単一の <see cref="SCCISharp::SoundInterfaceManager"/> クラスのインスタンスを取得します。
		/// </summary>
		/// <returns><see cref="SCCISharp::SoundInterfaceManager"/> オブジェクト。</returns>
		static SoundInterfaceManager^ GetInstance();

	internal:
		bool ReleaseInterface(IntPtr soundInterface);
		bool ReleaseSoundChip(IntPtr soundChip);

	private:
		bool isDisposed;

		SoundInterfaceManager();
		~SoundInterfaceManager();
		!SoundInterfaceManager();
		void CheckDisposed();

		static initonly Object^ lockObject = gcnew Object();
		static SoundInterfaceManager^ instance = nullptr;
	};

	/// <summary>
	/// インタフェースの機能をカプセル化したクラスです。
	/// </summary>
	public ref class SoundInterface : IDisposable
	{
	public:
		~SoundInterface();

		/// <summary>
		/// このインタフェースが低レベル API をサポートしているかの真偽値を取得します。
		/// </summary>
		/// <value>定例レベル API がサポートされているとき true、それ以外のとき false。</value>
		property bool IsSupportingLowLevelApi { bool get(); }

		/// <summary>
		/// ディレイ時間 (ms) を取得または設定します。
		/// </summary>
		/// <value>ディレイ時間を表す 0 以上の数値。</value>
		property Int32 Delay { Int32 get(); void set(Int32 value); }

		/// <summary>
		/// インタフェースに接続されているチップの総数を取得します。
		/// </summary>
		/// <value>チップの総数を表す 0 以上の数値。</value>
		property Int32 SoundChipCount { Int32 get(); }

		/// <summary>
		/// オブジェクトが解放されたかを表す真偽値を取得します。
		/// </summary>
		/// <value>解放されているとき true、それ以外のとき false。</value>
		property bool IsDisposed { bool get(); }

		/// <summary>
		/// このオブジェクトに割り当てられているポインタの値を取得します。
		/// </summary>
		/// <value>割り当てられているポインタの値。</value>
		property IntPtr Pointer { IntPtr get(); }

		/// <summary>
		/// 指定された配列をインタフェースに送信します。
		/// </summary>
		/// <param name="data">送信されるバイト列が格納された <see cref="System::Byte"/> の配列。</param>
		/// <param name="offset">送信が開始される配列のオフセット値。</param>
		/// <param name="length">送信されるバイト列の長さ。</param>
		/// <returns>送信に成功したとき true、それ以外のとき false。</returns>
		bool SendData(array<Byte>^ data, Int32 offset, Int32 length);

		/// <summary>
		/// インタフェースからデータを受信し、配列に格納します。
		/// </summary>
		/// <param name="data">受信されるバイト列を格納する <see cref="System::Byte"/> の配列。</param>
		/// <param name="offset">格納が開始される配列のオフセット値。</param>
		/// <param name="length">受信されるバイト列の長さ。</param>
		/// <returns>受信に成功し、配列に格納されたバイト列の長さ。</returns>
		Int32 ReceiveData(array<Byte>^ data, Int32 offset, Int32 length);

		/// <summary>
		/// インタフェースをリセットします。
		/// </summary>
		/// <returns>リセットに成功したとき true、それ以外のとき false。</returns>
		bool Reset();

		/// <summary>
		/// インタフェースを初期化します。
		/// </summary>
		/// <returns>初期化に成功したとき true、それ以外のとき false。</returns>
		bool Initialize();

		/// <summary>
		/// インタフェースに接続されているチップを表す <see cref="SCCISharp::SoundChip"/> オブジェクトを取得します。
		/// </summary>
		/// <param name="number">チップの番号。</param>
		/// <returns><see cref="SCCISharp::SoundChip"/> オブジェクト。</returns>
		SoundChip^ GetSoundChip(Int32 number);

	internal:
		SoundInterface(SoundInterfaceManager^ manager, void* soundInterface);

	private:
		!SoundInterface();

		bool isDisposed;

		SoundInterfaceManager^ manager;
		void* soundInterface;

		void CheckDisposed();
	};

	/// <summary>
	/// レジスタにデータ書き込まれたときに発生するイベントハンドラの引数を格納します。
	/// </summary>
	public ref class SetRegisterEventArgs : EventArgs
	{
	public:
		/// <summary>
		/// レジスタのアドレスを表す整数値を取得します。
		/// </summary>
		/// <value>レジスタのアドレスを表す 0 以上の整数値。</value>
		property int Address { int get(); }

		/// <summary>
		/// レジスタに書き込まれるデータの値を取得します。
		/// </summary>
		/// <value>レジスタに書き込まれるデータの値。</value>
		property int Data { int get(); }

		/// <summary>
		/// アドレスとデータを使って <see cref="SCCISharp::SetRegisterEventArgs"/> クラスのインスタンスを初期化しました。
		/// </summary>
		/// <param name="address">レジスタのアドレスを表す 0 以上の数値。</param>
		/// <param name="data">書き込まれるデータ。</param>
		SetRegisterEventArgs(int address, int data);

	private:
		int address;
		int data;
	};

	/// <summary>
	/// チップの機能をカプセル化したクラスです。
	/// </summary>
	public ref class SoundChip : IDisposable
	{
	public:
		~SoundChip();

		/// <summary>
		/// チップの種類を表す <see cref="SCCISharp::ChipType"/> の値を取得します。
		/// </summary>
		/// <value>チップの種類を表す <see cref="SCCISharp::ChipType"/> の値。</value>
		property ChipType Type { ChipType get(); }

		/// <summary>
		/// チップのクロック周波数 (Hz) を取得します。
		/// </summary>
		/// <value>クロック周波数 (Hz)。</value>
		property Int32 Clock { Int32 get(); }

		/// <summary>
		/// オブジェクトが解放されたかを表す真偽値を取得します。
		/// </summary>
		/// <value>解放されているとき true、それ以外のとき false。</value>
		property bool IsDisposed { bool get(); }

		/// <summary>
		/// このオブジェクトに割り当てられているポインタの値を取得します。
		/// </summary>
		/// <value>割り当てられているポインタの値。</value>
		property IntPtr Pointer { IntPtr get(); }

		/// <summary>
		/// チップの現在の状態を格納した <see cref="SCCISharp::SoundChipInfo"/> オブジェクトを取得します。
		/// </summary>
		/// <returns><see cref="SCCISharp::SoundChipInfo"/> オブジェクト。</returns>
		SoundChipInfo^ GetInfo();

		/// <summary>
		/// チップの指定したアドレスにデータを書き込みます。
		/// </summary>
		/// <param name="address">レジスタのアドレスを表す 0 以上の数値。</param>
		/// <param name="data">書き込まれるデータ。</param>
		/// <returns>書き込みに成功したとき true、それ以外のとき false。</returns>
		bool SetRegister(Int32 address, Int32 data);

		/// <summary>
		/// チップの指定したアドレスのデータを読み取ります。
		/// </summary>
		/// <param name="address">レジスタのアドレスを表す 0 以上の数値。</param>
		/// <returns>読み取られたレジスタのデータ値。</returns>
		Int32 GetRegister(Int32 address);

		/// <summary>
		/// チップを初期化します。
		/// </summary>
		/// <returns>初期化に成功したとき true、それ以外のとき false。</returns>
		bool Initialize();

		/// <summary>
		/// レジスタにデータ書き込まれたときに発生します。
		/// </summary>
		event EventHandler<SetRegisterEventArgs^>^ OnSetRegister;

	internal:
		SoundChip(SoundInterfaceManager^ manager, void* soundChip);

	private:
		!SoundChip();

		bool isDisposed;

		SoundInterfaceManager^ manager;
		void* soundChip;

		void CheckDisposed();
	};
}
