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
import MenuIcon from '@material-ui/icons/Menu';

const styles = theme => ({
  root: {
    flexGrow: 1,
  },
  menuButton: {
    marginRight: theme.spacing(2),
  },
  title: {
    flexGrow: 1,
  },
});

class MainAppBar extends Component {
  constructor() {
    super();

    this.state = {
      power: 0, 
    };
  }

  // Fetch current power status
  componentDidMount() {
    fetch('/api/status')
      .then((response) => response.json())
      .then((data) => 
        this.setState({
          power: data.status.powerOn
        })
      );
  }

  // Send GET request to API to toggle power to the LED strip
  togglePower() {
    fetch('/api/power/toggle')
      .then((response) => response.json())
      .then((data) => 
        this.setState({
          power: data.status.powerOn
        })
      );
  }

  // Determine the current power button color based on state
  buttonColor() {
    let color;

    switch(this.state.power) {
      case 0:
        color = "red";
        break;
      case 1:
        color = "green"
        break;
      default:
        color = "red";
        break;
    }

    return color;
  }

  render() {
    const { classes } = this.props;

    return (
      <div className={classes.root}>
        <AppBar position="static">
          <Toolbar>
            <IconButton edge="start" className={classes.menuButton} color="inherit" aria-label="menu">
              <MenuIcon />
            </IconButton>
            <Typography edge="start" variant="title" color="inherit" className={classes.title}>
              LED Strip Controls
            </Typography>
            <IconButton onClick={() => { this.togglePower() }}>
              <PowerSettingsNewIcon style={{fill: this.buttonColor()}}/>
            </IconButton>
          </Toolbar>
        </AppBar>
      </div>
    );
  }
}

export default withStyles(styles)(MainAppBar);
