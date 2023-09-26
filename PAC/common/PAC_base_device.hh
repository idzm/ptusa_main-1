/// @brief Базовые классы, которые реализуют функции передачи
/// состояния устройств PAC на сервер.
///

#pragma once

#include "param_ex.h"
#include "analog_emulator.h"
#include "bus_coupler_io.h"


const size_t MAX_COPY_SIZE = 1000;


//-----------------------------------------------------------------------------
/// @brief Устройство c параметрами.
///
/// Параметры хранятся в энергонезависимой памяти (сохраняют значение после
/// перезагрузки PAC). Доступ к параметрам производится на основе номера и
/// смещения (итоговый индекс равен их сумме). Каждый параметр имеет имя.
class par_device
    {
    friend class device;
    friend class PID;

    public:
        /// @param par_cnt - количество параметров.
        par_device( u_int par_cnt );

        virtual ~par_device();

        /// @brief Сохранение в виде скрипта Lua.
        ///
        /// Сохраняем на основе названий параметров в виде таблицы:
        /// имя_параметра = 'значение'.
        ///
        /// @param str - строка, куда сохраняем.
        int save_device( char* str );

        /// @brief Выполнение команда (установка значения параметра).
        ///
        /// @param name - имя команды (модифицируемого параметра).
        /// @param val - новое значение.
        ///
        /// @return 0 - ок.
        /// @return 1 - ошибка, параметр с таким именем не найден.
        int set_par_by_name( const char* name, double val );

        /// @brief Установка значения параметра.
        ///
        /// @param idx - индекс параметра (с единицы).
        /// @param offset - смещение индекса.
        /// @param value - новое значение.
        virtual void set_par( u_int idx, u_int offset, float value );

        /// @brief Установка значения рабочего параметра.
        ///
        /// @param idx - индекс рабочего параметра (с единицы).
        /// @param value - новое значение.
        virtual void set_rt_par( u_int idx, float value ) 
            {
            }

        /// @brief Получение значения параметра.
        ///
        /// @param idx - индекс параметра.
        /// @param offset - смещение индекса.
        ///
        /// @return значение параметра.
        float get_par( u_int idx, u_int offset = 0 ) const;

        /// @brief Задание имени параметра.
        ///
        /// @param idx - индекс параметра.
        /// @param offset - смещение индекса.
        /// @param name - имя параметра.
        void set_par_name( u_int idx, u_int offset, const char* name );

        /// @brief Получение параметров для хранения настроек ошибок устройства.
        saved_params_u_int_4* get_err_par() const
            {
            return err_par;
            }

        /// @brief Установка параметров для хранения настроек ошибок устройства.
        void set_err_par( saved_params_u_int_4* err_par )
            {
            this->err_par = err_par;
            }

        float get_par( int idx ) const
            {
            return par[ 0 ][ idx ];
            }

    private:
        /// @brief Ошибки устройства.
        saved_params_u_int_4* err_par;

        enum CONSTANTS
            {
            C_MAX_PAR_NAME_LENGTH = 25, ///< Максимальная длина имени параметра.
            };

        saved_params_float* par; ///< Параметры.
        char** par_name;         ///< Названия параметров.
    };
//-----------------------------------------------------------------------------
/// @brief Интерфейс счетчика.
class i_counter
    {
    public:
        virtual ~i_counter();

        /// @brief Приостановка работы счетчика.
        virtual void pause() = 0;

        /// @brief Возобновление работы счетчика.
        virtual void start() = 0;

        /// @brief Сброс счетчика и остановка счета.
        ///
        /// После сброса для продолжения работы необходимо вызвать @ref start().
        virtual void reset() = 0;

        /// @brief Сброс счетчика и продолжение счета.
        void restart();

        /// @brief Получение значения счетчика.
        virtual u_int get_quantity() = 0;

        /// @brief Получение значения счетчика.
        virtual float get_flow() = 0;

        /// @brief Получение состояния работы счетчика.
        virtual int get_state() = 0;

        /// @brief Получение абсолютного значения счетчика (без учета
        /// состояния паузы).
        virtual u_int get_abs_quantity() = 0;

        /// @brief Сброс абсолютного значения счетчика.
        virtual void  abs_reset() = 0;

        enum class STATES
            {
            S_WORK = 1,
            S_PAUSE,

            S_ERROR = -10,

            S_LOW_ERR = -12,
            S_HI_ERR = -13,
            };

    protected:
        /// @brief Получение времени ожидания работы насоса.
        virtual u_long get_pump_dt() const = 0;

        /// @brief Получение минимального расхода для учета работы связанных
        /// насосов.
        virtual float get_min_flow() const = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Интерфейс противосмешивающего клапана (mixproof).
class i_mix_proof
    {
    public:

        /// @brief Открыть верхнее седло.
        virtual void open_upper_seat() = 0;

        /// @brief Открыть нижнее седло.
        virtual void open_lower_seat() = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Интерфейс весов.
class i_wages
    {
    public:
        /// @brief Тарировка.
        virtual void tare() = 0;
        ///@brief Возвращает вес в килограммах
        virtual float get_value() = 0;
        ///@brief Возвращает состояние
        virtual int get_state() = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Устройство на основе дискретного входа.
///
/// Обратная связь, предельный уровень и т.д. являются примерами таких
/// устройств.
class i_DI_device : public i_cmd_device
    {
    public:
        /// @brief Получение состояния устройства.
        ///
        /// @return состояние устройства в виде целого числа.
        virtual int get_state() = 0;

        /// @brief Проверка активного состояния.
        ///
        /// Для уровня - сработал он или нет.
        ///
        /// @return 1 - устройство активно.
        /// @return 0 - устройство неактивно.
        virtual bool is_active();
    };
//-----------------------------------------------------------------------------
/// @brief Устройство на основе дискретного выхода.
///
/// Клапан, мешалка и т.д. являются примерами таких устройств.
class i_DO_device : public i_DI_device
    {
    public:
        /// @brief Включение устройства с учетом ручного режима.
        virtual void on();

        virtual void print() const = 0;

        /// @brief Выключение устройства с учетом ручного режима.
        virtual void off() = 0;

        /// @brief немедленное выключение устройства
        virtual void direct_off() = 0;

        /// @brief немедленное выключение устройства c учетом ручного режима
        virtual void instant_off();

        /// @brief Установка нового состояния устройства с учетом ручного режима.
        ///
        /// @param new_state - новое состояние устройства.
        virtual void set_state( int new_state );

    protected:
        /// @brief Установка нового состояния устройства.
        ///
        /// @param new_state - новое состояние объекта.
        virtual void direct_set_state( int new_state ) = 0;

        /// @brief Включение устройства.
        ///
        /// Установка устройства в активное состояние. Для клапана это означает
        /// его активирование, то есть если он нормально закрытый - открытие.
        virtual void direct_on() = 0;

        /// @brief Включен ли ручной режим.
        ///
        /// В ручном режиме устройство управляется по командам от сервера
        /// (управляющая программа не влияет на устройство).
        ///
        /// @return true - ручной режим включен.
        /// @return false - ручной режим выключен.
        virtual bool get_manual_mode() const = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Устройство на на основе аналогового входа.
///
/// Температура, расход и т.д. являются примерами таких устройств.
class i_AI_device : public i_cmd_device
    {
    public:
        /// @brief Получение текущего состояния устройства.
        ///
        /// @return - текущее состояние устройства в виде дробного числа.
        virtual float get_value() = 0;

        /// @brief Получение состояния устройства.
        ///
        /// @return состояние устройства в виде целого числа.
        virtual int get_state() = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Устройство на основе аналогового выхода.
///
/// Аналоговый канал управления и т.д. являются примерами таких устройств.
class i_AO_device : public i_AI_device
    {
    public:
        /// @brief Выключение устройства с учетом ручного режима.
        virtual void off() = 0;

        /// @brief Установка текущего состояния устройства с учетом ручного режима.
        virtual void set_value( float new_value );

    protected:
        /// @brief Установка текущего состояния устройства.
        ///
        /// @param new_value - новое состояние устройства.
        virtual void direct_set_value( float new_value ) = 0;

        /// @brief Включен ли ручной режим.
        ///
        /// В ручном режиме устройство управляется по командам от сервера
        /// (управляющая программа не влияет на устройство).
        ///
        /// @return true - ручной режим включен.
        /// @return false - ручной режим выключен.
        virtual bool get_manual_mode() const = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Интерфейс устройства как с аналоговыми, так и дискретными каналами.
class i_DO_AO_device : public i_AO_device, public i_DO_device
    {
    public:
        /// @brief Выключение устройства.
        ///
        /// Установка устройства в пассивное состояние. Для клапана это означает
        /// его деактивирование, то есть если он нормально закрытый - закрытие.
        virtual void off() = 0;

        /// @brief Получение состояния устройства.
        ///
        /// @return состояние устройства в виде целого числа.
        virtual int get_state() = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Класс универсального простого устройства, который используется в
/// режимах.
class device : public i_DO_AO_device, public par_device
    {
    public:
        /// @brief Выполнение команды.
        ///
        /// Запись в свойство объекта дробного числа.
        ///
        /// @param prop - свойство объекта.
        /// @param idx  - индекс свойства.
        /// @param val  - значение.
        virtual int set_cmd( const char* prop, u_int idx, double val );

        /// @brief Выполнение команды.
        ///
        /// Запись в свойство объекта строки.
        ///
        /// @param prop - свойство объекта.
        /// @param idx  - индекс свойства.
        /// @param val  - значение.
        virtual int set_cmd( const char* prop, u_int idx, char* val );

        /// @brief Сохранение устройства в виде скрипта Lua.
        ///
        /// @param prefix - префикс перед строкой скрипта (обычно символ
        /// табуляции - для визуального форматирования текста).
        /// @param buff [out] - буфер записи строки.
        virtual int save_device( char* buff, const char* prefix );

        /// @brief Расчет состояния на основе текущих данных от I/O.
        virtual void evaluate_io()
            {
            }

        enum CONSTANTS
            {
            C_MAX_NAME = 20,
            C_MAX_DESCRIPTION = 100
            };

        /// Типы устройств.
        enum DEVICE_TYPE
            {
            DT_NONE = -1,      ///< Тип не определен.

            DT_V = 0,   ///< Клапан.
            DT_VC,      ///< Управляемый клапан.
            DT_M,       ///< Двигатель.
            DT_LS,      ///< Уровень (есть/нет).
            DT_TE,      ///< Температура.
            DT_FS,      ///< Расход (есть/нет).
            DT_GS,      ///< Датчик положения.
            DT_FQT,     ///< Счетчик.
            DT_LT,      ///< Уровень (значение).
            DT_QT,      ///< Концентрация.

            DT_HA,      ///< Аварийная звуковая сигнализация.
            DT_HL,      ///< Аварийная световая сигнализация.
            DT_SB,      ///< Кнопка.
            DT_DI,      ///< Дискретный входной сигнал.
            DT_DO,      ///< Дискретный выходной сигнал.
            DT_AI,      ///< Аналоговый входной сигнал.
            DT_AO,      ///< Аналоговый выходной сигнал.
            DT_WT,      ///< Тензорезистор.
            DT_PT,      ///< Давление (значение).
            DT_F,       ///< Автоматический выключатель.
            DT_REGULATOR, ///< ПИД-регулятор.
            DT_HLA,      ///< Сигнальная колонна.
            DT_CAM,      ///< Камера.
            DT_PDS,      ///< Датчик разности давления.
            DT_TS,       ///< Сигнальный датчик температуры. 

            C_DEVICE_TYPE_CNT, ///< Количество типов устройств.
            };

        static const char* const DEV_NAMES[ device::DEVICE_TYPE::C_DEVICE_TYPE_CNT ];

        /// Подтипы устройств.
        enum DEVICE_SUB_TYPE
            {
            DST_NONE = -1,      ///< Подтип не определен.

            //V
            DST_V_DO1 = 1,         ///< Клапан с одним каналом управления.
            DST_V_DO2,             ///< Клапан с двумя каналами управления.
            DST_V_DO1_DI1_FB_OFF,  ///< Клапан с одним каналом управления и одной обратной связью на закрытое состояние.
            DST_V_DO1_DI1_FB_ON,   ///< Клапан с одним каналом управления и одной обратной связью на открытое состояние.
            DST_V_DO1_DI2,         ///< Клапан с одним каналом управления и двумя обратными связями.
            DST_V_DO2_DI2,         ///< Клапан с двумя каналами управления и двумя обратными связями.
            DST_V_MIXPROOF,        ///< Клапан противосмешивающий.
            DST_V_AS_MIXPROOF,     ///< Клапан с двумя каналами управления и двумя обратными связями с AS интерфейсом (противосмешивающий).
            DST_V_BOTTOM_MIXPROOF, ///< Донный клапан.
            DST_V_AS_DO1_DI2,      ///< Клапан с одним каналом управления и двумя обратными связями с AS интерфейсом.
            V_DO2_DI2_BISTABLE,    ///< Клапан с двумя каналами управления и двумя обратными связями бистабильный.

            V_IOLINK_VTUG_DO1,     ///< IO-Link VTUG8 клапан с одним каналом управления.

            V_IOLINK_VTUG_DO1_FB_OFF, ///< IO-Link VTUG клапан с одним каналом управления и одной обратной связью (выключенное состояние).
            V_IOLINK_VTUG_DO1_FB_ON,  ///< IO-Link VTUG клапан с одним каналом управления и одной обратной связью (включенное состояние).

            V_IOLINK_MIXPROOF,        ///< Клапан с двумя каналами управления и двумя обратными связями с IO-Link интерфейсом (противосмешивающий).
            V_IOLINK_DO1_DI2,         ///< Клапан с одним каналом управления и двумя обратными связями с IO-Link интерфейсом (отсечной).
            V_IOLINK_VTUG_DO1_DI2,    ///< IO-Link VTUG клапан с одним каналом управления и двумя обратными связями.

            DST_V_VIRT,               ///< Виртуальный клапан.

            DST_V_MINI_FLUSHING,      ///< Клапан с мини-клапаном промывки.

            ///< Противосмешивающий клапан (включение от IO-Link
            ///< пневмооострова) с тремя каналами управления.
            V_IOL_TERMINAL_MIXPROOF_DO3,

            ///< Противосмешивающий клапан (включение от IO-Link
            ///< пневмооострова) с тремя каналами управления и двумя обратными
            ///< связами.
            V_IOL_TERMINAL_MIXPROOF_DO3_DI2,

            //VC
            DST_VC = 1,         ///< Клапан с аналоговым управлением.
            DST_VC_IOLINK,      ///< Клапан IO-LInk.

            DST_VC_VIRT,        ///< Виртуальный клапан.

            //LS
            DST_LS_MIN = 1,     ///< Подключение по схеме минимум.
            DST_LS_MAX,         ///< Подключение по схеме максимум.

            LS_IOLINK_MIN,      ///< IOLInk уровень. Подключение по схеме минимум.
            LS_IOLINK_MAX,      ///< IOLInk уровень. Подключение по схеме максимум.

            DST_LS_VIRT,        ///< Виртуальный датчик уровня.

            //TE
            DST_TE = 1,
            DST_TE_IOLINK,      ///< Температура IO-LInk без дополнительных параметров.
            DST_TE_VIRT,        ///< Виртуальная температура.
            DST_TE_ANALOG,      ///< Температура AI c заданием пределов значения.

            //M,
            DST_M = 1,          ///< Мотор без управления частотой вращения.
            DST_M_FREQ,         ///< Мотор с управлением частотой вращения.

            DST_M_REV,          ///< Мотор с реверсом без управления частотой вращения. Реверс включается совместно.
            DST_M_REV_FREQ,     ///< Мотор с реверсом с управлением частотой вращения. Реверс включается совместно.
            DST_M_REV_2,        ///< Мотор с реверсом без управления частотой вращения. Реверс включается отдельно.
            DST_M_REV_FREQ_2,   ///< Мотор с реверсом с управлением частотой вращения. Реверс включается отдельно.


            /// Мотор с реверсом. Реверс включается отдельно. Отдельный сигнал
            /// ошибки.
            M_REV_2_ERROR,

            /// Мотор с реверсом с управлением частотой вращения. Реверс
            /// включается отдельно. Отдельный сигнал ошибки.
            DST_M_REV_FREQ_2_ERROR,

            /// Мотор, управляемый частотником Altivar. Связь с частотником по Ethernet.
            /// Реверс и аварии опциональны.
            M_ATV,

            DST_M_VIRT,        ///< Виртуальный мотор.

            /// Мотор, управляемый частотником Altivar. Связь с частотником по Ethernet.
            /// Реверс и аварии опциональны. Расчёт линейной скорости.
            M_ATV_LINEAR,

            //FQT
            DST_FQT = 1,        ///< Счетчик.
            DST_FQT_F,          ///< Счетчик + расход.
            ///< Резерв.
            DST_FQT_VIRT = 4,   ///Виртуальный cчетчик (без привязки к модулям).
            DST_FQT_IOLINK,     /// Счетчик IO-Link.

            //QT
            DST_QT = 1,   ///< Концентратомер.
            DST_QT_OK,    ///< Концентратомер c диагностикой.
            DST_QT_IOLINK,///Концентратомер IOLInk без дополнительных параметров.

            DST_QT_VIRT,  ///< Виртуальный концентратомер.

            //LT
            DST_LT = 1,    ///Текущий уровень без дополнительных параметров.
            DST_LT_CYL,    ///Текущий уровень для цилиндрического танка.
            DST_LT_CONE,   ///Текущий уровень для танка с конусом в основании.
            DST_LT_TRUNC,  ///Текущий уровень для танка с усеченным цилиндром в основании.

            DST_LT_IOLINK, ///Текущий IOLInk уровень без дополнительных параметров.

            DST_LT_VIRT,   ///< Виртуальный текущий уровень.

            //DO
            DST_DO = 1,    ///Обычный дискретный выход с привязкой к модулям
            DST_DO_VIRT,   ///Виртуальный дискретный выход(без привязки к модулям)

            //DI
            DST_DI = 1,    ///Обычный дискретный вход с привязкой к модулям
            DST_DI_VIRT,   ///Виртуальный дискретный вход(без привязки к модулям)

            //AO
            DST_AO = 1,    ///Обычный аналоговый выход с привязкой к модулям
            DST_AO_VIRT,   ///Виртуальный аналоговый выход(без привязки к модулям)

            //AI
            DST_AI = 1,    ///Обычный аналоговый вход с привязкой к модулям
            DST_AI_VIRT,   ///Виртуальный аналоговый вход(без привязки к модулям)

            //PT
            DST_PT = 1,      ///Обычный аналоговый датчик давления
            DST_PT_IOLINK,   ///Датчик давления IOLInk
            DST_PT_VIRT,     ///Виртуальный датчик давления

            //F
            DST_F = 1,       ///< Автоматический выключатель.
            DST_F_VIRT = 4,  ///< Виртуальный автоматический выключатель.

            //HLA
            DST_HLA = 1,  ///< Сигнальная колонна (красный, желтый, зеленый и сирена).
            DST_HLA_VIRT, ///< Виртуальная сигнальная колонна (без привязки к модулям).
            DST_HLA_IOLINK, ///< Сигнальная колонна IO-Link.

            //GS
            DST_GS = 1,  ///< Датчик положения.
            DST_GS_VIRT, ///< Виртуальный датчик положения (без привязки к модулям).

            //HA
            DST_HA = 1,  ///< Сирена.
            DST_HA_VIRT, ///< Виртуальная сирена (без привязки к модулям).

            //HL
            DST_HL = 1,  ///< Лампа.
            DST_HL_VIRT, ///< Виртуальная лампа (без привязки к модулям).

            //FS
            DST_FS = 1,  ///< Датчик наличия расхода.
            DST_FS_VIRT, ///< Виртуальный датчик наличия расхода (без привязки к модулям).

            //SB
            DST_SB = 1,  ///< Кнопка.
            DST_SB_VIRT, ///< Виртуальная кнопка (без привязки к модулям).

            //WT
            DST_WT = 1,  ///< Весы.
            DST_WT_VIRT, ///< Виртуальные весы.
            DST_WT_RS232,///< Весы c RS232 интерфейсом.
            DST_WT_ETH,  ///< Весы c интерфейсом ethernet.

            //CAM
            DST_CAM_DO1_DI2 = 1,///< C сигналом активации, результатом обработки и готовностью.
            DST_CAM_DO1_DI1,    ///< C сигналом активации и результатом обработки.
            DST_CAM_DO1_DI3,    ///< C сигналом активации, двумя результатами обработки и готовностью.

            //PDS
            DST_PDS = 1,
            DST_PDS_VIRT,

            //TS
            DST_TS = 1,
            DST_TS_VIRT,

            //DT_REGULATOR
            DST_REGULATOR_PID = 1,
            DST_REGULATOR_THLD,
            };

        device( const char* dev_name, device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type, u_int par_cnt );

        virtual ~device();

        const char* get_type_str() const;

        const char* get_type_name() const;

        const char* get_name() const
            {
            return name;
            }

        const char* get_description() const
            {
            return description;
            }

        virtual const char* get_error_description() const
            {
            return "обратная связь";
            }

        void set_descr( const char* new_description );

        virtual void set_article( const char* new_article );

        /// @brief Выключение устройства.
        ///
        /// Установка устройства в пассивное состояние. Для клапана это означает
        /// его деактивирование, то есть если он нормально закрытый - закрытие.
        virtual void direct_off() = 0;

        /// @brief Выключение устройства с учетом ручного режима.
        void off();

        /// @brief Вывод объекта в консоль.
        ///
        /// Для использования в отладочных целях.
        virtual void print() const;

        /// @brief Получение порядкового номера устройства.
        ///
        /// @return - номер устройства.
        virtual u_int_4 get_serial_n() const
            {
            return s_number;
            }

        /// @brief Установка порядкового номера устройства.
        ///
        /// @return - номер устройства.
        void set_serial_n( u_int_4 s_n )
            {
            s_number = s_n;
            }

        /// @brief Получение типа устройства.
        int get_type() const
            {
            return type;
            }

        /// @brief Получение подтипа устройства.
        int get_sub_type() const
            {
            return sub_type;
            }

        /// @brief Установка дополнительных свойств, значения которых -
        /// устройства.
        ///
        /// Для использования в Lua.
        virtual void set_property( const char* field, device* dev )
            {
            }

        /// @brief Установка дополнительных свойств, значения которых -
        /// строки.
        ///
        /// Для использования в Lua.
        virtual void set_string_property( const char* field, const char* value );

        /// @brief Сохранение дополнительных данных устройства в виде скрипта Lua.
        ///
        /// @param buff [out] - буфер записи строки.
        virtual int save_device_ex( char* buff )
            {
            buff[ 0 ] = 0;
            return 0;
            }

        bool get_manual_mode() const
            {
            return is_manual_mode;
            }

        char* get_article() const
            {
            return article;
            }

        void param_emulator( float math_expec, float stddev );

        bool is_emulation() const;

        void set_emulation( bool new_emulation_state );

        analog_emulator& get_emulator();

        /// @brief Получение максимальной длины имени устройства (с учётом 
        /// символа завершения строки).
        static int get_max_name_length()
            {
            return C_MAX_NAME;
            }

        /// @brief Получение максимальной длины описания устройства (с учётом 
        /// символа завершения строки).
        static int get_max_description_length()
            {
            return C_MAX_DESCRIPTION;
            }

    private:
        u_int_4 s_number;           ///< Последовательный номер устройства.

        DEVICE_TYPE     type;       ///< Тип устройства.
        DEVICE_SUB_TYPE sub_type;   ///< Подтип устройства.

        char* article;              ///< Артикул изделия.

        bool is_manual_mode;        ///< Признак ручного режима.

        char name[ C_MAX_NAME ];
        char* description;

        bool emulation = false;
        analog_emulator emulator;
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с дискретными входами/выходами.
///
/// Базовый класс для различных дискретных устройств.
class digital_io_device : public device,
    public io_device
    {
    public:
        digital_io_device( const char* dev_name, device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type, u_int par_cnt );

        virtual ~digital_io_device();

        float get_value() override;
        void direct_set_value( float new_value ) override;
        void direct_set_state( int new_state ) override;

        /// @brief Получение состояния объекта.
        ///
        /// @return - состояние объекта.
        int  get_state() override;

        void direct_on() override;
        void direct_off() override;

        virtual void print() const override;

    private:
        int state = 0;  ///< Дискретное состояние устройства.
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с аналоговыми входами/выходами.
///
/// Базовый класс для различных аналоговых устройств.
class analog_io_device : public device, public io_device
    {
    public:
        analog_io_device( const char* dev_name,
            device::DEVICE_TYPE type,
            device::DEVICE_SUB_TYPE sub_type,
            u_int par_cnt );

        void direct_set_state( int new_state ) override;
        int get_state() override;

        virtual void print() const override;
        void direct_on() override;
        void direct_off() override;

        int set_cmd( const char* prop, u_int idx, double val ) override;
        int save_device_ex( char* buff ) override;

        float get_value() override;
        void  direct_set_value( float new_value ) override;

    private:
        float value = .0f;    ///< Аналоговое состояние устройства.
    };
