/**
 * Material AppBar displayed at the top of each view
 */

import React, { Component } from "react";

import AppBar from '@material-ui/core/AppBar'
import Toolbar from '@material-ui/core/Toolbar'
import Typography from '@material-ui/core/Typography'
import IconButton from '@material-ui/core/IconButton';
import PowerSettingsNewIcon from '@material-ui/icons/PowerSettingsNew';
import PaletteIcon from '@material-ui/icons/Palette';
import WbIncandescentIcon from '@material-ui/icons/WbIncandescent';
import List from '@material-ui/core/List';
import ListItem from '@material-ui/core/ListItem';
import ListItemIcon from '@material-ui/core/ListItemIcon';
import ListItemText from '@material-ui/core/ListItemText';
import Divider from '@material-ui/core/Divider';
import Drawer from '@material-ui/core/Drawer';
import Hidden from '@material-ui/core/Hidden';
import { withStyles } from "@material-ui/core/styles";
import MenuIcon from '@material-ui/icons/Menu';

const drawerWidth = 240;

const styles = theme => ({
  root: {
    flexGrow: 1,
  },
  menuButton: {
    marginRight: theme.spacing(2),
    [theme.breakpoints.up('sm')]: {
      display: 'none',
    },
  },
  // necessary for content to be below app bar
  toolbar: theme.mixins.toolbar,
  drawerPaper: {
    width: drawerWidth,
  },
  content: {
    flexGrow: 1,
    padding: theme.spacing(3),
  },
  title: {
    flexGrow: 1,
  },
  appBar: {
    [theme.breakpoints.up('sm')]: {
      width: `calc(100% - ${drawerWidth}px)`,
      marginLeft: drawerWidth,
    },
  },
  drawer: {
    [theme.breakpoints.up('sm')]: {
      width: drawerWidth,
      flexShrink: 0,
    },
  },
});

class MainAppBar extends Component {
  constructor() {
    super();

    this.state = {
      power: 0, 
      mobileOpen: false
    };
  }

  // Toggle the side menu
  handleDrawerToggle() {
    this.setState(prevState => ({
      mobileOpen: !prevState.mobileOpen
    }));
  };

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

    const drawer = (
      <div>
        <div className={classes.toolbar} />
        <Divider />
        <List>
          <ListItem button key='animations'>
            <ListItemIcon> <WbIncandescentIcon /> </ListItemIcon>
            <ListItemText primary='Animations' />
          </ListItem>
  
          <ListItem button key='effects'>
            <ListItemIcon> <PaletteIcon /> </ListItemIcon>
            <ListItemText primary='Effects' />
          </ListItem>
        </List>
      </div>
    );

    return (
      <div className={classes.root}>
        <AppBar position="fixed" className={classes.appBar}>
          <Toolbar>
          <IconButton
            color="inherit"
            aria-label="open drawer"
            edge="start"
            onClick={() => { this.handleDrawerToggle() }}
            className={classes.menuButton}
          >
            <MenuIcon />
          </IconButton>
            <Typography edge="start" variant="subtitle1" color="inherit" className={classes.title}>
              LED Strip Controls
            </Typography>
            <IconButton onClick={() => { this.togglePower() }}>
              <PowerSettingsNewIcon style={{fill: this.buttonColor()}}/>
            </IconButton>
          </Toolbar>
        </AppBar>

        <nav className={classes.drawer} aria-label="mailbox folders">
        <Hidden smUp implementation="css">
          <Drawer
            container={container}
            variant="temporary"
            anchor='left'
            open={this.state.mobileOpen}
            onClose={() => { this.handleDrawerToggle() }}
            classes={{
              paper: classes.drawerPaper,
            }}
            ModalProps={{
              keepMounted: true, // Better open performance on mobile.
            }}
          >
            {drawer}
          </Drawer>
        </Hidden>
        <Hidden xsDown implementation="css">
          <Drawer
            classes={{
              paper: classes.drawerPaper,
            }}
            variant="permanent"
            open
          >
            {drawer}
          </Drawer>
        </Hidden>
      </nav>
      <main className={classes.content}>
        <div className={classes.toolbar} />

      </main>
      </div>
    );
  }
}

export default withStyles(styles)(MainAppBar);
