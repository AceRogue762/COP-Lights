/**
 * Material AppBar displayed at the top of each view
 */

import React, { Component } from "react";

import AppBar from '@material-ui/core/AppBar'
import Toolbar from '@material-ui/core/Toolbar'
import Typography from '@material-ui/core/Typography'
import IconButton from '@material-ui/core/IconButton';
import PowerSettingsNewIcon from '@material-ui/icons/PowerSettingsNew';
import { withStyles } from "@material-ui/core/styles";

const styles = theme => ({
  powerButton: {
    marginRight: 0,
  }, 
  title: {
    flexGrow: 1,
  }
});

class MainAppBar extends Component {
  constructor() {
    super();
  }

  render() {
    const { classes } = this.props;

    return (
      <div>
        <AppBar position="static">
            <Toolbar>
                <Typography variant="title" color="inherit" className={classes.powerButton}>
                  LED Strip Controls
                </Typography>
                <IconButton className={classes.powerButton}>
                  <PowerSettingsNewIcon color="secondary"/>
                </IconButton>
            </Toolbar>
        </AppBar>
      </div>
    );
  }
}

export default withStyles(styles)(MainAppBar);
