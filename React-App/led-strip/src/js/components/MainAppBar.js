/**
 * Material AppBar displayed at the top of each view
 */

import React, { Component } from "react";

import AppBar from '@material-ui/core/AppBar'
import Toolbar from '@material-ui/core/Toolbar'
import Typography from '@material-ui/core/Typography'

class MainAppBar extends Component {
  constructor() {
    super();
  }

  render() {
    return (
      <div>
        <AppBar>
            <Toolbar>
                <Typography variant="title" color="inherit">
                  LED Strip Controls
                </Typography>
            </Toolbar>
        </AppBar>
      </div>
    );
  }
}

export default MainAppBar;
