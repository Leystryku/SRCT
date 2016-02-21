#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <tchar.h> 
#include <msclr/marshal.h> 
#include <msclr\marshal_cppstd.h> 

#include "Form2.h"

extern char cmdbuffer[998];

extern bool bBuiltMenu;
extern bool bIsConnected;

extern int __stdcall MainThread();

bool createdgui = false;

class consolemsg
{
public:
	consolemsg(int red, int green, int blue, int alpha, char*txt)
	{
		r = red;
		g = green;
		b = blue;
		a = alpha;
		strcpy(text, txt);
	}
	int r;
	int g;
	int b;
	int a;
	char text[65502];

};

consolemsg *cachedconsolemsgs[0xFFF];
int curcachedconsolemsgs = 0;

namespace SRCT_cl {


	using namespace System;
	using namespace msclr::interop;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Zusammenfassung für Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{

	public:
		Form1(void)
		{
			InitializeComponent();
			InitializeVars();

			//
			//TODO: Konstruktorcode hier hinzufügen.
			//
		}

	protected:
		/// <summary>
		/// Verwendete Ressourcen bereinigen.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	public: System::Windows::Forms::RichTextBox^  richTextBox1;
	public: System::Windows::Forms::TextBox^  textBox1;

	protected:


	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::Button^  button1;
	private: Form2^ settings;

	protected:



	private:
		/// <summary>
		/// Erforderliche Designervariable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Erforderliche Methode für die Designerunterstützung.
		/// Der Inhalt der Methode darf nicht mit dem Code-Editor geändert werden.
		/// </summary>

		void InitializeVars(void)
		{
			bBuiltMenu = true;
			this->settings = (gcnew Form2());
		}
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->richTextBox1 = (gcnew System::Windows::Forms::RichTextBox());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->SuspendLayout();
			// 
			// richTextBox1
			// 
			this->richTextBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->richTextBox1->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(42)), static_cast<System::Int32>(static_cast<System::Byte>(41)),
				static_cast<System::Int32>(static_cast<System::Byte>(38)));
			this->richTextBox1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->richTextBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10));
			this->richTextBox1->ForeColor = System::Drawing::Color::White;
			this->richTextBox1->Location = System::Drawing::Point(0, 1);
			this->richTextBox1->Name = L"richTextBox1";
			this->richTextBox1->ReadOnly = true;
			this->richTextBox1->ScrollBars = System::Windows::Forms::RichTextBoxScrollBars::Vertical;
			this->richTextBox1->Size = System::Drawing::Size(534, 273);
			this->richTextBox1->TabIndex = 2;
			this->richTextBox1->Text = L"";
			this->richTextBox1->TextChanged += gcnew System::EventHandler(this, &Form1::richTextBox1_TextChanged_1);
			this->richTextBox1->MaxLength = 0xFFFFFFF;
			this->richTextBox1->WordWrap = true;
			this->richTextBox1->Invalidate();
			// 
			// button2
			// 
			this->button2->AccessibleName = L"";
			this->button2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->button2->AutoSize = true;
			this->button2->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"button2.Image")));
			this->button2->Location = System::Drawing::Point(504, 280);
			this->button2->Margin = System::Windows::Forms::Padding(1, 3, 3, 3);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(22, 23);
			this->button2->TabIndex = 4;
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &Form1::button2_Click_2);
			// 
			// button1
			// 
			this->button1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->button1->AutoSize = true;
			this->button1->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"button1.Image")));
			this->button1->Location = System::Drawing::Point(478, 280);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(22, 23);
			this->button1->TabIndex = 3;
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &Form1::button1_Click_1);
			// 
			// textBox1
			// 
			this->textBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10));
			this->textBox1->HideSelection = false;
			this->textBox1->Location = System::Drawing::Point(2, 280);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(470, 23);
			this->textBox1->TabIndex = 5;
			this->textBox1->WordWrap = false;
			this->textBox1->TextChanged += gcnew System::EventHandler(this, &Form1::textBox1_TextChanged_1);
			
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(42)), static_cast<System::Int32>(static_cast<System::Byte>(41)),
				static_cast<System::Int32>(static_cast<System::Byte>(38)));
			this->ClientSize = System::Drawing::Size(529, 322);
			this->Controls->Add(this->textBox1);
			this->Controls->Add(this->richTextBox1);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->button1);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->MaximumSize = System::Drawing::Size(10000, 10000);
			this->MinimumSize = System::Drawing::Size(260, 184);
			this->Name = L"Form1";
			this->Text = L"SRCT - Not connected";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->ResumeLayout(false);
			this->PerformLayout();
			this->Refresh();
			Timer^ timer = gcnew System::Windows::Forms::Timer();
			timer->Interval = 50;
			timer->Tick += gcnew System::EventHandler(this, &Form1::richtimer);
			timer->Start();

			Timer^ timer2 = gcnew System::Windows::Forms::Timer();
			timer2->Interval = 3;
			timer2->Tick += gcnew System::EventHandler(this, &Form1::richtimer2);
			timer2->Start();
			createdgui = true;
			

			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainThread, 0, 0, 0);
		}
#pragma endregion
	private: System::Void richTextBox1_TextChanged(System::Object^  sender, System::EventArgs^  e) {
	}

	bool shouldcaret = false;

	private: System::Void textBox1_TextChanged(System::Object^  sender, System::EventArgs^  e) {

	}
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
	}
	private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {
	}
	private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) {
	}
	private: System::Void richTextBox1_TextChanged_1(System::Object^  sender, System::EventArgs^  e) {
		
		//richTextBox1->SelectionStart = richTextBox1->TextLength;
		//
		shouldcaret = true;
	}

	public: void AddTexts()
	{
		if (!curcachedconsolemsgs)
			return;
		
		if (!richTextBox1 || !richTextBox1->Text)
			return;

		for (int i = 1; i < curcachedconsolemsgs+1; i++)
		{
			

			consolemsg *curmsg = cachedconsolemsgs[i];
			if (!curmsg || !curmsg->text)
				continue;

			if (curmsg->r < 0 || curmsg->r>255)
				continue;

			if (curmsg->g < 0 || curmsg->g>255)
				continue;

			if (curmsg->b < 0 || curmsg->b>255)
				continue;

			if (curmsg->a < 0 || curmsg->a>255)
				continue;

			richTextBox1->SelectionStart = richTextBox1->TextLength;
			richTextBox1->SelectionLength = 0;

			richTextBox1->SelectionColor = Color::FromArgb(curmsg->a, curmsg->r, curmsg->g, curmsg->b);
			richTextBox1->AppendText(marshal_as<System::String^>(curmsg->text));
			richTextBox1->SelectionColor = richTextBox1->ForeColor;
			delete[] curmsg;

		}
		
		curcachedconsolemsgs = 0;

	}

	private: System::Void richtimer(System::Object^  sender, System::EventArgs^  e) {


		AddTexts();

	}


	private: System::Void ScrollToEnd(System::Windows::Forms::RichTextBox^ rtb)
	{
		IntPtr *ptrWparam = new IntPtr(SB_BOTTOM);
		IntPtr *ptrLparam = new IntPtr(0);
		SendMessage((HWND)(void*)rtb->Handle, WM_VSCROLL, (WPARAM)ptrWparam, (LPARAM)ptrLparam);
	}

	private: System::Void richtimer2(System::Object^  sender, System::EventArgs^  e) {

		if (shouldcaret)
		{
			shouldcaret = false;
			//richTextBox1->SelectionStart = richTextBox1->Text->Length; //Set the current caret position at the end
			//richTextBox1->ScrollToCaret();
			//PostMessageW(richTextBox1->Handle, WM_VSCROLL, (IntPtr)SB_BOTTOM, (IntPtr)IntPtr.Zero);
			richTextBox1->Focus();
			ScrollToEnd(richTextBox1);
			textBox1->Focus();
		}

	}

	public: void AddConsoleText(int r, int g, int b, int a, char *newtext)
	{
	
		curcachedconsolemsgs = curcachedconsolemsgs + 1;

		cachedconsolemsgs[curcachedconsolemsgs] = new consolemsg(r, g, b, a, newtext);

		consolemsg *consolemsg = cachedconsolemsgs[curcachedconsolemsgs];
	}

	private: System::Void button1_Click_1(System::Object^  sender, System::EventArgs^  e) {
		
		for (int i = 1; i < 4000; i++)
			richTextBox1->AppendText("SHIT\n");

		if ( bIsConnected )
			EnterInputText();

	}
	private: System::Void button1_Click_2(System::Object^  sender, System::EventArgs^  e) {
		if ( bIsConnected )
			EnterInputText();

	}
	private: System::Void button2_Click_1(System::Object^  sender, System::EventArgs^  e) {
		if (!this->settings) this->settings = (gcnew Form2());

		if (this->settings->Visible)
		{
			this->settings->Visible = 0;
			return;
		}

		this->settings->Visible = 1;

	}
	private: System::Void button2_Click_2(System::Object^  sender, System::EventArgs^  e) {
		if ( !this->settings ) this->settings = (gcnew Form2());

		if (this->settings->Visible)
		{
			this->settings->Visible = 0;
			return;
		}
		
		this->settings->Visible = 1;

	}

	private: System::Void textBox1_TextChanged_1(System::Object^  sender, System::EventArgs^  e) {

	}



	public: void EnterInputText()
	{
		if (!textBox1 || !textBox1->Text || !textBox1->Text->Length ) return;
		if ( textBox1->Text->Length < 1 ) return;


		marshal_context^ context = gcnew marshal_context();
		const char* cmdinput = context->marshal_as<const char*>(textBox1->Text);

		memset(cmdbuffer, 0, sizeof(cmdbuffer));
		strcpy_s(cmdbuffer, cmdinput);

		AddConsoleText(128,128,128,255, cmdbuffer);
		AddConsoleText(255,255,255,255, "\n");

		textBox1->Text = "";

	}
	
};


}