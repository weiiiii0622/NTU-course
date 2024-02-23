import { useState } from 'react';
import Box from '@material-ui/core/Box'
import Button from '@material-ui/core/Button';
import FormControl from '@material-ui/core/FormControl';
import FormControlLabel from '@material-ui/core/FormControlLabel';
import Paper from '@material-ui/core/Paper';
import Radio from '@material-ui/core/Radio';
import RadioGroup from '@material-ui/core/RadioGroup';
import styled from 'styled-components';
import TextField from '@material-ui/core/TextField';
import Typography from '@material-ui/core/Typography';
import { Tabs, Tab} from '@material-ui/core';
import DenseTable from './Densetable';

import { useStyles } from '../hooks';
import axios from '../api';
import { useScoreCard } from '../hooks/useScoreCard';

const Wrapper = styled.section`
  display: flex;
  width: 80vw;
  flex-direction: column;
`;

const Row = styled.div`
  display: flex;
  align-items: center;
  justify-content: center;
  width: 100%;
  padding: 1em;
`;

const StyledFormControl = styled(FormControl)`
  min-width: 120px;
`;

const ContentPaper = styled(Paper)`
  height: 300px;
  padding: 2em;
  overflow: auto;
`;

function TabPanel(props) {
  const { children, value, index, ...other } = props;

  return (
    <div
      role="tabpanel"
      hidden={value !== index}
      id={`simple-tabpanel-${index}`}
      aria-labelledby={`simple-tab-${index}`}
      {...other}
    >
      {value === index && (
        <Box sx={{ p: 3 }}>
          <Typography>{children}</Typography>
        </Box>
      )}
    </div>
  );
}


const Body = () => {
  const classes = useStyles();

  const { value, messages, add_messages, query_messages, add_rows, query_rows, setAddRows, setQueryRows, setValue, addCardMessage, addRegularMessage, addErrorMessage } =
    useScoreCard();

  // Tab
  const handleTabChange = (event, newValue) => {
    setValue(newValue);
  };

  // Table
  const createData = (name, subject, score) => {
    return { name, subject, score };
  }

  const [name, setName] = useState('');
  const [subject, setSubject] = useState('');
  const [score, setScore] = useState(0);

  const [queryType, setQueryType] = useState('name');
  const [queryString, setQueryString] = useState('');

  const handleChange = (func) => (event) => {
    func(event.target.value);
  };

  const handleAdd = async () => {
    const {
      data: { message, card, record },
    } = await axios.post('/card', {
      name,
      subject,
      score,
    });

    setValue(1);
    if (!card) addErrorMessage(message, "add");
    else{
      addCardMessage(message);
    }
    var nRows = []
    record.forEach((r) => {
      nRows.push(createData(r.name, r.subject, r.score));
    });
    setAddRows([...nRows]);
  };

  const handleQuery = async () => {
    console.log("query....");
    console.log(queryType);
    console.log(queryString);
    const {
      data: { messages, message },
    } = await axios.post('/cards', {
      type: queryType,
      queryString,
    });
    console.log("query finished....");
    console.log(messages);
    console.log(message);
    setValue(2);
    setQueryRows([]);
    if (!messages) addErrorMessage(message, "query");
    else{
      addRegularMessage(`Found card with ${queryType} ${queryString}:`);
      var nRows = []
      messages.forEach((msg) => {
        nRows.push(createData(msg.name, msg.subject, msg.score));
      });
      setQueryRows([...nRows]);
    } 
  };

  return (
    <Wrapper>

      <Box sx={{ borderBottom: 1, borderColor: 'divider' }}>
        <Tabs value={value} onChange={handleTabChange}>
          <Tab label="Main"/>
          <Tab label="Add" />
          <Tab label="Query"/>
        </Tabs>
      </Box>

      <TabPanel value={value} index={0}>
        <ContentPaper variant="outlined">
          {messages.map((m, i) => (
            <Typography variant="body2" key={m + i} style={{ color: m.color }}>
              {m.message}
            </Typography>
          ))}
        </ContentPaper>
      </TabPanel>

      <TabPanel value={value} index={1}>
        <Row>
          {/* Could use a form & a library for handling form data here such as Formik, but I don't really see the point... */}
          <TextField
            className={classes.input}
            placeholder="Name"
            value={name}
            onChange={handleChange(setName)}
          />
          <TextField
            className={classes.input}
            placeholder="Subject"
            style={{ width: 240 }}
            value={subject}
            onChange={handleChange(setSubject)}
          />
          <TextField
            className={classes.input}
            placeholder="Score"
            value={score}
            onChange={handleChange(setScore)}
            type="number"
          />
          <Button
            className={classes.button}
            variant="contained"
            color="primary"
            disabled={!name || !subject}
            onClick={handleAdd}
          >
            Add
          </Button>
        </Row>
        <ContentPaper variant="outlined">
          {add_messages.map((m, i) => (
            <Typography variant="body2" key={m + i} style={{ color: m.color }}>
              {m.message}
            </Typography>
          ))}
          <DenseTable rows={add_rows}></DenseTable>
        </ContentPaper>
      </TabPanel>

      <TabPanel value={value} index={2}>
        <Row>
          <StyledFormControl>
            <FormControl component="fieldset">
              <RadioGroup
                row
                value={queryType}
                onChange={handleChange(setQueryType)}
              >
                <FormControlLabel
                  value="name"
                  control={<Radio color="primary" />}
                  label="Name"
                />
                <FormControlLabel
                  value="subject"
                  control={<Radio color="primary" />}
                  label="Subject"
                />
              </RadioGroup>
            </FormControl>
          </StyledFormControl>
          <TextField
            placeholder="Query string..."
            value={queryString}
            onChange={handleChange(setQueryString)}
            style={{ flex: 1 }}
          />
          <Button
            className={classes.button}
            variant="contained"
            color="primary"
            disabled={!queryString}
            onClick={handleQuery}
          >
            Query
          </Button>
        </Row>
        <ContentPaper variant="outlined">
          {query_messages.map((m, i) => (
            <Typography variant="body2" key={m + i} style={{ color: m.color }}>
              {m.message}
            </Typography>
          ))}
          <DenseTable rows={query_rows}></DenseTable>
        </ContentPaper>
      </TabPanel>

    </Wrapper>
  );
};

export default Body;
