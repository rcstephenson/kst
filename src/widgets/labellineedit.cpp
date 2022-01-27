/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "labellineedit.h"

#include <QDebug>

namespace Kst {

LabelLineEdit::LabelLineEdit(QWidget *parent)
  : QLineEdit(parent) {
  setWhatsThis(tr("<qt>The syntax for labels is a derivative of a subset of LaTeX.  "
                    "Supported syntax is: <b>\\[greeklettername]</b> and <b>\\[Greeklettername]</b>, "
                    "<b>\\approx</b>, <b>\\cdot</b>, <b>\\ge</b>, <b>\\geq</b>, <b>\\inf</b> ,"
                    "<b>\\int</b>, <b>\\le</b>, <b>\\leq</b>, <b>\\ne</b>, <b>\\n</b>, "
                    "<b>\\partial</b>, <b>\\prod</b>, <b>\\pm</b>, "
                    "<b>\\textcolor{color name}{colored text}</b>, <b>\\textbf{bold text}</b>, "
                    "<b>\\textit{italicized text}</b>, <b>\\t</b>, <b>\\sum</b>, <b>\\sqrt</b>, "
                    "<b>\\underline{underlined text}</b>, <b>\\overline{overlined text}</b>, "
                    "<b>x^y</b>, <b>x_y</b>.  "
                    "Scalars, equations, and vector elements can be embedded.  "
                    "Scalar: <i>[V1/Mean]</i>.  Vector Element: <i>[V1[4]]</i>.  "
                    "Equation: <i>[=[V1/Mean]^2]</i>.  A [ character can be inserted as <i>\\[</i>."));

  setToolTip(tr("Label text.  A subset of LaTeX is supported.  See What's This."));
}


LabelLineEdit::~LabelLineEdit() {
}

void LabelLineEdit::focusInEvent ( QFocusEvent * event ) {
  emit inFocus();
  QLineEdit::focusInEvent(event);
}

}

// vim: ts=2 sw=2 et
