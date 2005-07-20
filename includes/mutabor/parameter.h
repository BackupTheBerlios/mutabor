/** \file
 ********************************************************************
 * Alles zu Parametern.
 *
 * \author Tobias Schlemmer <keinstein_junior@gmx.net>
 * \date 2005
 * \version 0.1
 ********************************************************************/

#ifndef __PARAMETER_H_INCLUDED
#define __PARAMETER_H_INCLUDED


#ifdef __cplusplus
namespace mutabor {
  extern "C" {
#endif

    /** Einfach verkettete Liste mit Parameternamen */
    struct parameter_liste {
      char * name;                    /**< Name des Parameters */
      struct parameter_liste * next;  /**< Nächster Parameter */
    };

    /** Ganzzahliger Parametertyp */
    typedef int parameter_typ;

    void init_parameter_liste (void);
    void get_new_name_in_parameterlist (char * name);
    void get_new_number_in_parameterlist (double wert);

    int parameter_list_laenge (struct parameter_liste *list);

    int parameter_nummer (int aktueller_index, char * such_name, 
			  struct parameter_liste * knoten);
    struct parameter_liste * get_last_parameter_liste (void);

#ifdef __cplusplus 
  } // extern "C"
} // namespace mutabor
#endif

#endif /* __PARAMETER_H_INCLUDED */