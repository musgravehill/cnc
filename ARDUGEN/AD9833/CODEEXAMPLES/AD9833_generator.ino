#include <SPI.h>
// ***** I2C дисплей *****
#include <LiquidCrystal_I2C.h> // https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
#define cols 20
#define rows 4
LiquidCrystal_I2C lcd(0x27, cols, rows);
char *Blank;

// ********** AD9833 **********
#define bMode 0x2
#define bDiv2 0x8
#define bOpbiten 0x20
#define bSleep12 0x40
#define bSleep1 0x80
#define bReset 0x100
#define bHLB 0x1000
#define bB28 0x2000
#define bCntrl_reg 0x0
#define bFreq_reg0 0x4000
#define bFreq_reg1 0x8000
#define bPhase_reg 0xC000

unsigned long Freq;
unsigned long FreqStep;
int Phase;
enum eWaveForm {wfSin, wfTri, wfSqr, wfSqr2};
eWaveForm WaveForm;

// ********** Параметры меню **********
#define ShowScrollBar 1     // Показывать индикаторы прокрутки (0/1)
#define ScrollLongCaptions 1// Прокручивать длинные названия (0/1)
#define ScrollDelay 800     // Задержка при прокрутке текста
#define BacklightDelay 20000// Длительность подсветки
#define ReturnFromMenu 1    // Выходить из меню после выбора элемента(0/1)

enum eMenuKey {mkNull, mkBack, mkRoot, mkSetFreq, mkSetPhase, mkForm, mkSin, mkTri, mkSquare,
               mkSquareDiv2, mkInc, mkInc1, mkInc10, mkInc100, mkInc1000, mkAbout
              };

// ********** Переменные для энкодера ***************
#define pin_CLK 7 // Энкодер пин A
#define pin_DT  6 // Энкодер пин B
#define pin_Btn 5 // Кнопка

unsigned long CurrentTime, PrevEncoderTime;
enum eEncoderState {eNone, eLeft, eRight, eButton};
eEncoderState EncoderState;
int EncoderA, EncoderB, EncoderAPrev, counter;
bool ButtonPrev;

// ********** Прототипы функций ***************
eEncoderState GetEncoderState();
void LCDBacklight(byte v = 2);
eMenuKey DrawMenu(eMenuKey Key);

// ******************** Меню ********************
byte ScrollUp[8]  = {0x4, 0xa, 0x11, 0x1f};
byte ScrollDown[8]  = {0x0, 0x0, 0x0, 0x0, 0x1f, 0x11, 0xa, 0x4};

byte ItemsOnPage = rows;    // Максимальное количество элементов для отображения на экране
unsigned long BacklightOffTime = 0;
unsigned long ScrollTime = 0;
byte ScrollPos;
byte CaptionMaxLength;

struct sMenuItem {
  eMenuKey  Parent;       // Ключ родителя
  eMenuKey  Key;          // Ключ
  char      *Caption;     // Название пункта меню
  void      (*Handler)(); // Обработчик
};

sMenuItem Menu[] = {
  {mkNull, mkRoot, "Menu", NULL},
    {mkRoot, mkSetFreq, "Set frequency", NULL},
    {mkRoot, mkSetPhase, "Set phase", NULL},
    {mkRoot, mkForm, "Set wave form", NULL},
      {mkForm, mkSin, "Sinusoidal", NULL},
      {mkForm, mkTri, "Triangular", NULL},
      {mkForm, mkSquare, "Square", NULL},
      {mkForm, mkSquareDiv2, "Square F/2", NULL},
      {mkForm, mkBack, "Back", NULL},
    {mkRoot, mkInc, "Set frequency increment value", NULL},
      {mkInc, mkInc1, "1 HZ", NULL},
      {mkInc, mkInc10, "10 HZ", NULL},
      {mkInc, mkInc100, "100 HZ", NULL},
      {mkInc, mkInc1000, "1000 HZ", NULL},
      {mkInc, mkBack, "Back", NULL},
    {mkRoot, mkAbout, "About", NULL},
    {mkRoot, mkBack, "Back", NULL}
};

const int MenuLength = sizeof(Menu) / sizeof(Menu[0]);

void LCDBacklight(byte v) { // Управление подсветкой
  if (v == 0) { // Выключить подсветку
    BacklightOffTime = millis();
    lcd.noBacklight();
  }
  else if (v == 1) { //Включить подсветку
    BacklightOffTime = millis() + BacklightDelay;
    lcd.backlight();
  }
  else { // Выключить если время вышло
    if (BacklightOffTime < millis())
      lcd.noBacklight();
    else
      lcd.backlight();
  }
}

eMenuKey DrawMenu(eMenuKey Key) { // Отрисовка указанного уровня меню и навигация по нему
  eMenuKey Result;
  int k, l, Offset, CursorPos, y;
  sMenuItem **SubMenu = NULL;
  bool NeedRepaint;
  String S;
  l = 0;
  LCDBacklight(1);
  // Запишем в SubMenu элементы подменю
  for (byte i = 0; i < MenuLength; i++) {
    if (Menu[i].Key == Key) {
      k = i;
    }
    else if (Menu[i].Parent == Key) {
      l++;
      SubMenu = (sMenuItem**) realloc (SubMenu, l * sizeof(void*));
      SubMenu[l - 1] = &Menu[i];
    }
  }

  if (l == 0) { // l==0 - подменю нет
    if ((ReturnFromMenu == 0) and (Menu[k].Handler != NULL)) (*Menu[k].Handler)(); // Вызываем обработчик если он есть
    LCDBacklight(1);
    return Key; // и возвращаем индекс данного пункта меню
  }

  // Иначе рисуем подменю
  CursorPos = 0;
  Offset = 0;
  ScrollPos = 0;
  NeedRepaint = 1;
  do {
    if (NeedRepaint) {
      NeedRepaint = 0;
      lcd.clear();
      y = 0;
      for (int i = Offset; i < min(l, Offset + ItemsOnPage); i++) {
        lcd.setCursor(1, y++);
        lcd.print(String(SubMenu[i]->Caption).substring(0, CaptionMaxLength));
      }
      lcd.setCursor(0, CursorPos);
      lcd.print(">");
      if (ShowScrollBar) {
        if (Offset > 0) {
          lcd.setCursor(cols - 1, 0);
          lcd.write(0);
        }
        if (Offset + ItemsOnPage < l) {
          lcd.setCursor(cols - 1, ItemsOnPage - 1);
          lcd.write(1);
        }
      }
    }
    EncoderState = GetEncoderState();
    switch (EncoderState) {
      case eLeft: {
          // Прокрутка меню вверх
          LCDBacklight(1);
          ScrollTime = millis() + ScrollDelay * 5;
          if (CursorPos > 0) {  // Если есть возможность, поднимаем курсор
            if ((ScrollLongCaptions) and (ScrollPos)) {
              // Если предыдущий пункт меню прокручивался, то выводим его заново
              lcd.setCursor(1, CursorPos);
              lcd.print(Blank);
              lcd.setCursor(1, CursorPos);
              lcd.print(String(SubMenu[Offset + CursorPos]->Caption).substring(0, CaptionMaxLength));
              ScrollPos = 0;
            }
            // Стираем курсор на старом месте, рисуем в новом
            lcd.setCursor(0, CursorPos--);
            lcd.print(" ");
            lcd.setCursor(0, CursorPos);
            lcd.print(">");
          }
          else if (Offset > 0) {
            //Курсор уже в крайнем положении. Если есть пункты выше, то перерисовываем меню
            Offset--;
            NeedRepaint = 1;
          }
          break;
        }
      case eRight: {
          // Прокрутка меню вниз
          LCDBacklight(1);
          ScrollTime = millis() + ScrollDelay * 5;
          if (CursorPos < min(l, ItemsOnPage) - 1) {// Если есть возможность, то опускаем курсор
            if ((ScrollLongCaptions) and (ScrollPos)) {
              // Если предыдущий пункт меню прокручивался, то выводим его заново
              lcd.setCursor(1, CursorPos);
              lcd.print(Blank);
              lcd.setCursor(1, CursorPos);
              lcd.print(String(SubMenu[Offset + CursorPos]->Caption).substring(0, CaptionMaxLength));
              ScrollPos = 0;
            }
            // Стираем курсор на старом месте, рисуем в новом
            lcd.setCursor(0, CursorPos++);
            lcd.print(" ");
            lcd.setCursor(0, CursorPos);
            lcd.print(">");
          }
          else {
            // Курсор уже в крайнем положении. Если есть пункты ниже, то перерисовываем меню
            if (Offset + CursorPos + 1 < l) {
              Offset++;
              NeedRepaint = 1;
            }
          }
          break;
        }
      case eButton: {
          // Выбран элемент меню. Нажатие кнопки Назад обрабатываем отдельно
          LCDBacklight(1);
          ScrollTime = millis() + ScrollDelay * 5;
          if (SubMenu[CursorPos + Offset]->Key == mkBack) {
            free(SubMenu);
            return mkBack;
          }
          Result = DrawMenu(SubMenu[CursorPos + Offset]->Key);
          if ((Result != mkBack) and (ReturnFromMenu)) {
            free(SubMenu);
            return Result;
          }
          NeedRepaint = 1;
          break;
        }
      case eNone: {
          if (ScrollLongCaptions) {
            // При бездействии прокручиваем длинные названия
            S = SubMenu[CursorPos + Offset]->Caption;
            if (S.length() > CaptionMaxLength)
            {
              if (ScrollTime < millis())
              {
                ScrollPos++;
                if (ScrollPos == S.length() - CaptionMaxLength)
                  ScrollTime = millis() + ScrollDelay * 2; // Небольшая задержка когда вывели все название
                else if (ScrollPos > S.length() - CaptionMaxLength)
                {
                  ScrollPos = 0;
                  ScrollTime = millis() + ScrollDelay * 5; // Задержка перед началом прокрутки
                }
                else
                  ScrollTime = millis() + ScrollDelay;
                lcd.setCursor(1, CursorPos);
                lcd.print(Blank);
                lcd.setCursor(1, CursorPos);
                lcd.print(S.substring(ScrollPos, ScrollPos + CaptionMaxLength));
              }
            }
          }
          LCDBacklight();
        }
    }
  } while (1);
}
//****************************************

void setup() {
  pinMode(pin_CLK, INPUT);
  pinMode(pin_DT,  INPUT);
  pinMode(pin_Btn, INPUT_PULLUP);
  SPI.begin();
  lcd.begin();
  lcd.backlight();
  CaptionMaxLength = cols - 1;
  Blank = (char*) malloc(cols * sizeof(char));
  for (byte i = 0; i < CaptionMaxLength; i++)
    Blank[i] = ' ';
  if (ShowScrollBar) {
    CaptionMaxLength--;
    lcd.createChar(0, ScrollUp);
    lcd.createChar(1, ScrollDown);
  }
  Blank[CaptionMaxLength] = 0;
  FreqStep = 1;
  WaveForm = wfSin;
  WriteAD9833(bCntrl_reg | bReset | bB28);
  SetFrequency(100); // Установим частоту 100Гц
  SetPhase(0); // Сдвиг по фазе 0
  WriteAD9833(bCntrl_reg | bB28); // Снимаем Reset
  LCDRepaint(); // Выведем текущие параметры на дисплей
  LCDBacklight(1); // Включаем подвсветку
}

void loop() {
  unsigned long F;
  // В цикле опрашиваем энкодер
  switch (GetEncoderState()) {
    case eNone: { // При бездействии отключаем подсветку по таймауту
        LCDBacklight();
        return;
      }
    case eLeft: { // Уменьшить частоту
        LCDBacklight(1); // Включаем подсветку
        if (Freq == 1) return; // Меньше уже некуда
        if (Freq > FreqStep)
          SetFrequency(Freq - FreqStep);
        else
          SetFrequency(1);
        break;
      }
    case eRight: { // Увеличить частоту
        LCDBacklight(1); // Включаем подсветку
        if (Freq == 12500000) return; // Больше уже некуда
        if (Freq + FreqStep <= 12500000)
          SetFrequency(Freq + FreqStep);
        else
          SetFrequency(12500000);
        break;
      }
    case eButton: { // При нажатии на кнопку показываем меню
        LCDBacklight(1); // Включаем подсветку
        switch (DrawMenu(mkRoot)) { // Показываем меню и анализируем выбранный пункт
          case mkSetFreq: { // Установить новое значение частоты
              F = InputFreq(); // Запрашиваем новое значение
              while ((F > 12500000) or (F < 1)) {
                // Частота должна быть в диапазоне 1...12500000Гц
                lcd.clear();
                lcd.print("Frequency should be");
                lcd.setCursor(0, 1);
                lcd.print("between 1Hz and");
                lcd.setCursor(0, 2);
                lcd.print("12.5Mhz");
                while (GetEncoderState() == eNone);
                F = InputFreq(); // Предлагаем ввести частоту повторно
              }
              if (F != Freq)
                SetFrequency(F); // Устанавливаем новое значение частоты
              break;
            }
          case mkSetPhase:   { // Установить фазу
              SetPhase(InputPhase());
              break;
            }
          case mkSin:        { // Изменить форму сигнала на синусоидальную
              SetForm(wfSin);
              break;
            }
          case mkTri:        { // ... треугольную
              SetForm(wfTri);
              break;
            }
          case mkSquare:     { // ... прямоугольную
              SetForm(wfSqr);
              break;
            }
          case mkSquareDiv2: { // ... прямоугольную с делителем на 2
              SetForm(wfSqr2);
              break;
            }
          case mkInc1:    {    // Установить приращение частоты 1Гц
              FreqStep = 1;
              break;
            }
          case mkInc10:   {
              FreqStep = 10;  // 10Гц
              break;
            }
          case mkInc100:  {
              FreqStep = 100; // 100Гц
              break;
            }
          case mkInc1000: {
              FreqStep = 1000;// 1000Гц
              break;
            }
          case mkAbout: {
              lcd.clear();
              lcd.print("AD9833 generator");
              lcd.setCursor(0, 1);
              lcd.print("tsibrov.blogspot.com");
              while (GetEncoderState() == eNone){
                LCDBacklight();
              }
              LCDBacklight(1);
              break;
            }
        }
        // После выхода из меню перерисовываем главный экран
        LCDRepaint();
        return;
      }
  }
  // Обновить значение частоты
  lcd.setCursor(0, 0);
  lcd.print(Blank);
  lcd.setCursor(0, 0);
  lcd.print("Freq:  ");
  lcd.print(Freq);
  lcd.print("Hz");
}

// ******************** Вывод информации на дисплей ********************
void LCDRepaint() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Freq:  ");
  lcd.print(Freq);
  lcd.print("Hz");
  lcd.setCursor(0, 1);
  lcd.print("Phase: ");
  lcd.print(Phase);
  lcd.print("Deg");
  lcd.setCursor(0, 2);
  lcd.print("Form:  ");
  switch (WaveForm) {
    case wfSin: {
        lcd.print("Sinusoidal");
        break;
      }
    case wfTri: {
        lcd.print("Triangular");
        break;
      }
    case wfSqr: {
        lcd.print("Square");
        break;
      }
    case wfSqr2: {
        lcd.print("Square F/2");
        break;
      }
  }
}

// ******************** Энкодер с кнопкой ********************
eEncoderState GetEncoderState() {
  // Считываем состояние энкодера
  eEncoderState Result = eNone;
  CurrentTime = millis();
  if (CurrentTime >= (PrevEncoderTime + 5)) {
    PrevEncoderTime = CurrentTime;
    if (digitalRead(pin_Btn) == LOW ) {
      if (ButtonPrev) {
        Result = eButton; // Нажата кнопка
        ButtonPrev = 0;
      }
    }
    else {
      ButtonPrev = 1;
      EncoderA = digitalRead(pin_DT);
      EncoderB = digitalRead(pin_CLK);
      if ((!EncoderA) && (EncoderAPrev)) { // Сигнал A изменился с 1 на 0
        if (EncoderB) Result = eRight;     // B=1 => энкодер вращается по часовой
        else          Result = eLeft;      // B=0 => энкодер вращается против часовой
      }
      EncoderAPrev = EncoderA; // запомним текущее состояние
    }
  }
  return Result;
}

// ******************** Ввод нового значения частоты ********************
unsigned long InputFreq() {
  unsigned long F = Freq;
  int Positions[] = {4, 5, 7, 8, 9, 11, 12, 13};
  int Digits[8];
  int p = 0;
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Input frequency:");
  lcd.setCursor(14, 1);
  lcd.print("Hz");
  // Разбиваем частоту на разряды и выводим на дисплей
  for (int i = 7; i >= 0; i--) {
    Digits[i] = F % 10;
    lcd.setCursor(Positions[i], 1);
    lcd.print(Digits[i]);
    F = F / 10;
  }
  lcd.setCursor(3, 3);
  lcd.print("OK      Cancel");
  lcd.setCursor(Positions[0], 1);
  lcd.cursor();

  //Основной цикл - выбор разряда для изменения либо OK/Cancel
  while (1)
  {
    EncoderState = GetEncoderState();
    switch (EncoderState) {
      case eNone: {
          LCDBacklight();
          continue;
        }
      case eLeft: { // Двигаем курсор влево
          LCDBacklight(1); // Включаем подсветку
          if (p == 0) continue; // Левее перемещаться некуда
          if (p == 9) { // Выбран Cancel, перемещаемся к OK
            lcd.setCursor(10, 3); lcd.print(' ');
            lcd.setCursor(2, 3);  lcd.print('>');
            p--;
            continue;
          }
          if (p == 8) { // Выбран OK, перемещаемся к частоте
            lcd.setCursor(2, 3); lcd.print(' ');
            p--;
            lcd.setCursor(Positions[p], 1);
            lcd.cursor();
            continue;
          }
          // Выбрана частота, перемещаемся к старшему разряду
          p--;
          lcd.setCursor(Positions[p], 1);
          continue;;
        }
      case eRight: { // Двигаем курсор вправо
          LCDBacklight(1); // Включаем подсветку
          if (p == 9) continue; // Правее перемещаться некуда
          if (p == 8) { // Выбран Ok, перемещаемся к Cancel
            lcd.setCursor(2, 3); lcd.print(' ');
            lcd.setCursor(10, 3); lcd.print('>');
            p++;
            continue;
          }
          if (p == 7) { // Выбран младший разряд частоты, перемещаемся к OK
            lcd.noCursor();
            lcd.setCursor(2, 3); lcd.print('>');
            p++;
            continue;
          }
          // Выбрана частота, перемещаемся к младшему разряду
          p++;
          lcd.setCursor(Positions[p], 1);
          continue;;
        }
      case eButton: { //Нажата кнопка
          LCDBacklight(1); // Включаем подсветку
          if (p == 9) {
            lcd.noCursor();
            return Freq; // Cancel.
          }
          if (p == 8) { // OK. Собираем и возвращаем новое значение частоты
            lcd.noCursor();
            F = 0;
            for (int i = 0; i < 8; i++)
              F = F * 10 + Digits[i];
            return F;
          }
          // Редактирование выбранного разряда частоты
          EncoderState = eNone;
          lcd.setCursor(Positions[p], 1);
          lcd.blink();
          while (EncoderState != eButton)
          {
            EncoderState = GetEncoderState();
            switch (EncoderState) {
              case eNone: {
                  LCDBacklight();
                  continue;
                }
              case eLeft: {
                  LCDBacklight(1); // Включаем подсветку
                  if (Digits[p] == 0) continue;
                  lcd.setCursor(Positions[p], 1);
                  lcd.print(--Digits[p]);
                  lcd.setCursor(Positions[p], 1);
                  continue;
                }
              case eRight: {
                  LCDBacklight(1); // Включаем подсветку
                  if (Digits[p] == 9) continue;
                  lcd.setCursor(Positions[p], 1);
                  lcd.print(++Digits[p]);
                  lcd.setCursor(Positions[p], 1);
                  continue;
                }
            }
          }
          LCDBacklight(1);
          lcd.noBlink();
          continue;
        }
    }
  }
}

// ******************** Ввод фазы ********************
int InputPhase() {
  int w = Phase;
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Input phase shift:");
  lcd.setCursor(6, 1);
  lcd.print(w);
  lcd.print("Deg");
  while (1) {
    EncoderState = GetEncoderState();
    switch (EncoderState) {
      case eNone: {
          LCDBacklight();
          continue;
        }
      case eButton: {
          LCDBacklight(1);
          return w;
      }
      case eLeft: {
          LCDBacklight(1);
          if (w > 0) w--;
          break;
        }
      case eRight: {
          LCDBacklight(1);
          if (w < 360) w++;
          break;
        }
    }
    lcd.setCursor(6, 1);
    lcd.print(w);
    lcd.print("Deg  ");
  }
}

// ******************** Установить частоту ********************
void SetFrequency(unsigned long val) {
  Freq = val;
  unsigned long FreqData = round((float) val * 10.73741 + 0.5);
  WriteAD9833(FreqData & 0x3FFF | bFreq_reg0);
  WriteAD9833((FreqData >> 14) | bFreq_reg0);
}

// ******************** Установить фазу ********************
void SetPhase(int val) {
  Phase = val;
  unsigned long PhaseData = round (float(val) * 11.37777 + 0.5);
  WriteAD9833(PhaseData | bPhase_reg);
}

// ******************** Установить форму ********************
void SetForm(eWaveForm val) {
  WaveForm = val;
  int16_t CntrlData;
  switch (val) {
    case wfSin: {
        CntrlData = 0;
        break;
      }
    case wfTri: {
        CntrlData = bMode;
        break;
      }
    case wfSqr: {
        CntrlData = bOpbiten | bDiv2 | bSleep12;
        break;
      }
    case wfSqr2: {
        CntrlData = bOpbiten | bSleep12;
        break;
      }
  }
  WriteAD9833(CntrlData | bCntrl_reg | bB28);
}

// ******************** Передача 16-битного слова в AD9833 ********************
void WriteAD9833(uint16_t Data) {
  SPI.beginTransaction(SPISettings(SPI_CLOCK_DIV8, MSBFIRST, SPI_MODE2));
  digitalWrite(SS, LOW);
  delayMicroseconds(1);
  SPI.transfer16(Data);
  digitalWrite(SS, HIGH);
  SPI.endTransaction();
}
