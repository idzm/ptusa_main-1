#include "PAC_base_device.hh"

#include <fmt/core.h>

const char* const device::DEV_NAMES[ device::DEVICE_TYPE::C_DEVICE_TYPE_CNT ] =
    {
    "V",       ///< Клапан.
    "VC",      ///< Управляемый клапан.
    "M",       ///< Двигатель.
    "LS",      ///< Уровень (есть/нет).
    "TE",      ///< Температура.
    "FS",      ///< Расход (есть/нет).
    "GS",      ///< Датчик положения.
    "FQT",     ///< Счетчик.
    "LT",      ///< Уровень (значение).
    "QT",      ///< Концентрация.

    "HA",      ///< Аварийная звуковая сигнализация.
    "HL",      ///< Аварийная световая сигнализация.
    "SB",      ///< Кнопка.
    "DI",      ///< Дискретный входной сигнал.
    "DO",      ///< Дискретный выходной сигнал.
    "AI",      ///< Аналоговый входной сигнал.
    "AO",      ///< Аналоговый выходной сигнал.
    "WT",      ///< Тензорезистор.
    "PT",      ///< Давление (значение).
    "F",       ///< Автоматический выключатель.

    "C",       ///< ПИД-регулятор.
    "HLA",     ///< Сигнальная колонна.
    "CAM",     ///< Камера.
    "PDS",     ///< Датчик разности давления.
    "TS",      ///< Сигнальный датчик температуры.
    };
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int par_device::save_device( char* str )
    {
    str[ 0 ] = 0;

    if ( par == 0 )
        {
        return 0;
        }

    int size = 0;
    for ( u_int i = 0; i < par->get_count(); i++ )
        {
        if ( par_name[ i ] )
            {
            auto val = par[ 0 ][ i + 1 ];
            double tmp;
            int precision = modf( val, &tmp ) == 0 ? 0 : 2;
            size += fmt::format_to_n( str + size, MAX_COPY_SIZE, "{}={:.{}f}, ",
                par_name[ i ], val, precision ).size;
            }
        }
    return size;
    }
//-----------------------------------------------------------------------------
int par_device::set_par_by_name( const char* name, double val )
    {
    if ( par )
        {
        for ( u_int i = 0; i < par->get_count(); i++ )
            {
            if ( strcmp( par_name[ i ], name ) == 0 )
                {
                par->save( i + 1, (float)val );
                return 0;
                }
            }
        }

    if ( G_DEBUG )
        {
        printf( "par_device::set_cmd() - name = %s wasn't found.\n",
            name );
        }
    return 1;
    }
//-----------------------------------------------------------------------------
void par_device::set_par( u_int idx, u_int offset, float value )
    {
    if ( par )
        {
        par[ 0 ][ offset + idx ] = value;
        }
    }
//-----------------------------------------------------------------------------
par_device::par_device( u_int par_cnt ) : err_par( 0 ), par( 0 ),
par_name( 0 )
    {
    if ( par_cnt )
        {
        par_name = new char* [ par_cnt ];
        for ( u_int i = 0; i < par_cnt; i++ )
            {
            par_name[ i ] = 0;
            }

        par = new saved_params_float( par_cnt );
        }
    }
//-----------------------------------------------------------------------------
par_device::~par_device()
    {
    if ( par )
        {
        for ( u_int i = 0; i < par->get_count(); i++ )
            {
            delete[] par_name[ i ];
            par_name[ i ] = nullptr;
            }

        delete[] par_name;
        par_name = nullptr;

        delete par;
        par = nullptr;
        }
    }
//-----------------------------------------------------------------------------
float par_device::get_par( u_int idx, u_int offset ) const
    {
    if ( par )
        {
        return par[ 0 ][ offset + idx ];
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void par_device::set_par_name( u_int idx, u_int offset, const char* name )
    {
    if ( par )
        {
        if ( offset + idx <= par->get_count() && ( offset + idx > 0 ) )
            {
            if ( strlen( name ) > C_MAX_PAR_NAME_LENGTH )
                {
                if ( G_DEBUG )
                    {
                    printf( "Error par_device::set_par_name( u_int idx, u_int offset, const char* name ) - "
                        "name length (%zu) > param C_MAX_PAR_NAME_LENGTH (%d).",
                        strlen( name ), C_MAX_PAR_NAME_LENGTH );
                    }

                return;
                }

            if ( 0 == par_name[ offset + idx - 1 ] )
                {
                par_name[ offset + idx - 1 ] = new char[ strlen( name ) + 1 ];
                strcpy( par_name[ offset + idx - 1 ], name );
                }
            else
                {
                if ( G_DEBUG )
                    {
                    printf( "Error par_device::set_par_name (u_int idx, u_int offset, const char* name) - "
                        "param (%d %d) already has name (%s).",
                        offset, idx, par_name[ offset + idx - 1 ] );
                    }
                }
            }
        else
            {
            if ( G_DEBUG )
                {
                printf( "Error par_device::set_par_name (u_int idx, u_int offset, const char* name) - "
                    "offset (%d) + idx (%d) > param count ( %d ).",
                    offset, idx, par->get_count() );
                }
            }
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void device::set_descr( const char* new_description )
    {
    delete[] description;

    //Копирование с учетом нуль-символа.
    int len = strlen( new_description ) + 1;
    description = new char[ len ];
    strcpy( description, new_description );
    }
//-----------------------------------------------------------------------------
void device::set_article( const char* new_article )
    {
    delete[] article;

    //Копирование  с учетом нуль-символа.
    int len = strlen( new_article ) + 1;
    article = new char[ len ];
    strcpy( article, new_article );
    }
//-----------------------------------------------------------------------------
void device::print() const
    {
    printf( "%s\t", name );
    }
//-----------------------------------------------------------------------------
void device::off()
    {
    if ( !get_manual_mode() )
        {
        direct_off();
        }
    }
//-----------------------------------------------------------------------------
void device::set_string_property( const char* field, const char* value )
    {
    if ( G_DEBUG )
        {
        G_LOG->debug( "%s\t device::set_string_property() - "
            "field = %s, val = \"%s\"",
            name, field, value );
        }
    }
//-----------------------------------------------------------------------------
int device::save_device( char* buff, const char* prefix )
    {
    int res = fmt::format_to_n( buff, MAX_COPY_SIZE,
        "{}{}={{M={:d}, ", prefix, name, is_manual_mode ).size;

    if ( type != DT_AO )
        {
        res += fmt::format_to_n( buff + res, MAX_COPY_SIZE, "ST={}, ",
            get_state() ).size;
        }

    if ( type != DT_V &&

        type != DT_FS &&
        type != DT_GS &&

        type != DT_HA &&
        type != DT_HL &&
        type != DT_SB &&
        !( type == DT_LS && ( sub_type == DST_LS_MAX || sub_type == DST_LS_MIN ) ) &&

        type != DT_DI &&
        type != DT_DO )
        {
        auto val = get_value();
        double tmp;
        int precision = modf( val, &tmp ) == 0 ? 0 : 2;
        res += fmt::format_to_n( buff + res, MAX_COPY_SIZE, "V={:.{}f}, ",
            val, precision ).size;
        }

    res += save_device_ex( buff + res );
    res += par_device::save_device( buff + res );

    const int extra_symbols_length = 2;                     //Remove last " ,".
    if ( res > extra_symbols_length ) res -= extra_symbols_length;
    res += fmt::format_to_n( buff + res, MAX_COPY_SIZE, "}},\n" ).size;
    return res;
    }
//-----------------------------------------------------------------------------
int device::set_cmd( const char* prop, u_int idx, char* val )
    {
    if ( G_DEBUG )
        {
        printf( "%s\t device::set_cmd() - prop = %s, idx = %d, val = %s\n",
            name, prop, idx, val );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int device::set_cmd( const char* prop, u_int idx, double val )
    {
    if ( G_DEBUG )
        {
        sprintf( G_LOG->msg,
            "%s\t device::set_cmd() - prop = %s, idx = %d, val = %f",
            name, prop, idx, val );
        G_LOG->write_log( i_log::P_DEBUG );
        }

    switch ( prop[ 0 ] )
        {
        case 'S':
            direct_set_state( (int)val );
            break;

        case 'V':
            direct_set_value( (float)val );
            break;

        case 'M':
            is_manual_mode = val == 0. ? false : true;
            break;

        case 'P': //Параметры.
            return par_device::set_par_by_name( prop, val );

        default:
            if ( G_DEBUG )
                {
                printf( "Error device::set_cmd() - prop = %s, val = %f\n",
                    prop, val );
                }
            return 1;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
device::device( const char* dev_name, DEVICE_TYPE type, DEVICE_SUB_TYPE sub_type,
    u_int par_cnt ) : par_device( par_cnt ), s_number( 0 ), type( type ),
    sub_type( sub_type ), is_manual_mode( false ), article( 0 )
    {
    if ( dev_name )
        {
        strcpy( this->name, dev_name );
        }
    else
        {
        strcpy( this->name, "?" );
        }

    description = new char[ 1 ];
    description[ 0 ] = 0;

    article = new char[ 2 ];
    article[ 0 ] = ' ';
    article[ 1 ] = 0;
    }
//-----------------------------------------------------------------------------
const char* device::get_type_str() const
    {
    return DEV_NAMES[ type ];
    }
//-----------------------------------------------------------------------------
const char* device::get_type_name() const
    {
    switch ( type )
        {
        case DT_V:
            return "Клапан";
        case DT_VC:
            return "Управляемый клапан";
        case DT_M:
            return "Двигатель";

        case DT_LS:
            return "Уровень";
        case DT_TE:
            return "Температура";
        case DT_FS:
            return "Расход";
        case DT_GS:
            return "Датчик положения";
        case DT_FQT:
            return "Счетчик";
        case DT_LT:
            return "Уровень";
        case DT_QT:
            return "Концентрация";
        case DT_HA:
            return "Аварийная звуковая сигнализация";
        case DT_HL:
            return "Аварийная световая сигнализация";
        case DT_SB:
            return "Кнопка";
        case DT_DI:
            return "Дискретный входной сигнал";
        case DT_DO:
            return "Дискретный выходной сигнал";
        case DT_AI:
            return "Аналоговый входной сигнал";
        case DT_AO:
            return "Аналоговый выходной сигнал";
        case DT_WT:
            return "Тензорезистор";
        case DT_PT:
            return "Давление";
        case DT_F:
            return "Автоматический выключатель";
        case DT_REGULATOR:
            return "ПИД-регулятор";
        case DT_HLA:
            return "Сигнальная колонна";
        case DT_CAM:
            return "Камера";

        default:
            return "???";
        }
    }
//-----------------------------------------------------------------------------
void device::param_emulator( float math_expec, float stddev )
    {
    emulator.param( math_expec, stddev );
    }
//-----------------------------------------------------------------------------
analog_emulator& device::get_emulator()
    {
    return emulator;
    }
//-----------------------------------------------------------------------------
bool device::is_emulation() const
    {
    return emulation;
    }
//-----------------------------------------------------------------------------
void device::set_emulation( bool new_emulation_state )
    {
    emulation = new_emulation_state;
    }
//-----------------------------------------------------------------------------
device::~device()
    {
    delete[] description;
    description = nullptr;
    delete[] article;
    article = nullptr;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------